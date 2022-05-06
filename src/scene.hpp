#pragma once

#include "math/vec3.hpp"
#include <vector>
#include <string>
#include "triangle.hpp"

namespace XmlRaytracer {

struct Camera {
    Vec3 position;
    Vec3 gaze;
    Vec3 up;
    double l, r, t, b;
    double distance;
    int nx, ny;
};

struct Light {
    int id;
    Vec3 position;
    Vec3 intensity;
};

struct Material {
    int id;
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    int phong_exponent;
    Vec3 mirror_reflectance;
};

struct Mesh {
    int id;
    int material_id;
    std::vector<Vec3> faces;
};

struct Scene {
    int max_raytrace_depth;
    Color3 background;
    Camera camera;
    Color3 ambient_light;
    std::vector<Light> lights;
    std::vector<Material> materials;
    std::vector<Vec3> vertex_data;
    std::vector<Mesh> objects;

    HitResult
    hit(const Ray& ray, double t_min, double t_max, bool abort_on_hit) const;

    const Material& find_material(int id) const;
};

} // namespace XmlRaytracer
