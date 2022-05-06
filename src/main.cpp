#include "dev.h"
#include <fmt/core.h>
#include <iostream>
#include <tinyxml2.h>
#include "fileio/ppm.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "scene.hpp"
#include "fileio/xml_scene_parser.hpp"
#include "triangle.hpp"
#include <cmath>
#include <chrono>
#include <future>

namespace XmlRaytracer {

Color3 ray_color(const Ray& ray, const Scene& scene, int depth) {
    if (depth > scene.max_raytrace_depth) {
        return {0.0, 0.0, 0.0};
    }

    HitResult hr = scene.hit(ray, 0, infinity, false);
    if (!hr.is_hit) {
        return scene.background;
    }

    const Material& material = scene.find_material(hr.material_id);
    Vec3 calculated_light = scene.ambient_light * material.ambient;

    Vec3 cam_vector = unit_vector(-ray.d);

    // light calculation
    for (const auto& light : scene.lights) {
        Vec3 light_vector = light.position - hr.point;
        double light_distance = light_vector.length();

        // shadows
        Ray shadow_ray{hr.point + (light_vector * 0.0000001), light_vector};
        HitResult hr_shadow = scene.hit(shadow_ray, 0, infinity, true);
        double shadow_hit_distance = (hr_shadow.point - shadow_ray.o).length();
        if (hr_shadow.is_hit && shadow_hit_distance <= light_distance) {
            continue;
        }

        // diffuse shading
        double normal_dot_light = dot(hr.normal, light_vector);
        double cos_theta =
            normal_dot_light / (hr.normal.length() * light_vector.length());
        if (normal_dot_light > 0) {
            calculated_light +=
                (light.intensity / (light_distance * light_distance)) *
                cos_theta * material.diffuse;
        }

        // specular shading
        Vec3 half_vector = light_vector + cam_vector;
        half_vector /= half_vector.length();
        double normal_dot_half = dot(hr.normal, half_vector);
        double cos_alpha =
            normal_dot_half / (hr.normal.length() * half_vector.length());
        cos_alpha = pow(cos_alpha, material.phong_exponent);
        if (normal_dot_half > 0) {
            calculated_light +=
                (light.intensity / (light_distance * light_distance)) *
                cos_alpha * material.specular;
        }
    }

    // recursive reflection
    if (!(material.mirror_reflectance.x <= 0.0 &&
          material.mirror_reflectance.y <= 0.0 &&
          material.mirror_reflectance.z <= 0.0)) {
        double cos_theta_reflect = dot(hr.normal, cam_vector) /
                                   (hr.normal.length() * cam_vector.length());
        Vec3 reflect_vector = (2 * hr.normal * cos_theta_reflect) - cam_vector;
        Ray next_ray{hr.point + (reflect_vector * 0.000001), reflect_vector};
        Color3 reflect_color = ray_color(next_ray, scene, depth + 1);
        calculated_light += material.mirror_reflectance * reflect_color;
    }

    // clamp result in 0...255
    for (int i = 0; i < 3; i++) {
        if (calculated_light[i] > 255) {
            calculated_light[i] = 255;
        } else if (calculated_light[0] < 0) {
            calculated_light[i] = 0;
        }
    }
    return calculated_light;
}

struct ThreadReturn {
    std::vector<PixelData> pixels;
    int start;
};

ThreadReturn thread_job(const Scene& scene, int pixel_start, int pixel_end) {
    const Camera& cam = scene.camera;
    const Vec3& v = cam.up;
    const Vec3& w = -cam.gaze;
    const Vec3& u = cross(v, w);
    const Vec3& e = cam.position;
    const double& d = cam.distance;
    Vec3 image_center = e + (-w * d);
    Vec3 image_corner = image_center + cam.t * v + cam.l * u;

    std::vector<PixelData> pixels{};

    for (int i = pixel_start; i < pixel_end; i++) {
        int y = i / cam.nx;
        int x = i % cam.nx;

        double su = (x + 0.5) * ((cam.r - cam.l) / cam.nx);
        double sv = (y + 0.5) * ((cam.t - cam.b) / cam.ny);
        Vec3 s = image_corner + su * u - sv * v;
        Ray ray{e, s - e};

        Color3 color = ray_color(ray, scene, 0);
        pixels.push_back({color});
    }

    fmt::print("ThreadJob [{}...{}] out {} pixels\n",
               pixel_start,
               pixel_end,
               pixels.size());

    return {pixels, pixel_start};
}

}; // namespace XmlRaytracer

int main(int arg, char const* args[]) {
    auto start = std::chrono::high_resolution_clock::now();

    if (arg != 2) {
        fmt::print("Correct usage of the program is: \"./program "
                   "[path-to-scene-xml]\"");
        return -1;
    }

    const char* scene_xml_path = args[1];

    using namespace XmlRaytracer;

    Scene scene;
    if (!create_scene_from_xml(scene_xml_path, scene)) {
        fmt::print("Scene couldn't be created from given xml: {}\n",
                   scene_xml_path);
        return -1;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds total_time{};
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    total_time = duration;
    fmt::print("Scene xml loaded from {} in: {}ms\n",
               scene_xml_path,
               duration.count());

    start = std::chrono::high_resolution_clock::now();

    ImageData img{scene.camera.nx, scene.camera.ny, {}};

    const int thread_count =
        static_cast<int>(std::thread::hardware_concurrency());
    const int pixel_count = scene.camera.nx * scene.camera.ny;
    const int pixel_per_thread = pixel_count / thread_count;
    const int pixel_left_over = pixel_count % thread_count;
    fmt::print("Thread count: {}\n", thread_count);
    fmt::print("Pixel count: {}\n", pixel_count);
    fmt::print("Pixel per thread: {}\n", pixel_per_thread);
    fmt::print("Pixel left over: {}\n", pixel_left_over);

    std::mutex mutex;
    std::condition_variable cond_vars;

    std::vector<std::future<ThreadReturn>> futures{};
    for (int i = 0; i + pixel_per_thread <= pixel_count - pixel_left_over;
         i += pixel_per_thread) {
        fmt::print(
            "Creating thread for pixels [{}...{}]\n", i, i + pixel_per_thread);
        auto future =
            std::async(std::launch::async,
                       [pixel_per_thread, i, &scene]() -> ThreadReturn {
                           return thread_job(scene, i, i + pixel_per_thread);
                       });
        futures.push_back(std::move(future));
    }

    fmt::print("Processing left-over pixels [{}...{}]\n",
               pixel_count - pixel_left_over,
               pixel_count);
    auto left_over_data =
        thread_job(scene, pixel_count - pixel_left_over, pixel_count);

    {
        std::unique_lock<std::mutex> lock(mutex);
        cond_vars.wait(lock, [&futures, &thread_count] {
            return futures.size() == static_cast<size_t>(thread_count);
        });
    }

    std::vector<ThreadReturn> results{};
    for (auto& future : futures) {
        ThreadReturn res = future.get();
        results.push_back(res);
    }

    sort(results.begin(),
         results.end(),
         [](const ThreadReturn& a, const ThreadReturn& b) -> bool {
             return a.start < b.start;
         });

    for (auto& res : results) {
        for (auto& pixel : res.pixels) {
            img.pixels.push_back(pixel);
        }
    }
    for (auto& pixel : left_over_data.pixels) {
        img.pixels.push_back(pixel);
    }

    stop = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    total_time += duration;
    fmt::print("Rendering took: {}ms\n", duration.count());

    start = std::chrono::high_resolution_clock::now();
    img.write_ppm("out.ppm");
    stop = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    total_time += duration;
    fmt::print("Writing PPM file took: {}ms\n", duration.count());
    fmt::print("Total program execution time: {}ms\n", total_time.count());

    return 0;
}
