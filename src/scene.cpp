#include "scene.hpp"

namespace XmlRaytracer {

const Material& Scene::find_material(int id) const {
    auto it = find_if(materials.begin(),
                      materials.end(),
                      [&](const Material& m) { return m.id == id; });
    return *it;
}

HitResult Scene::hit(const Ray& ray,
                     double t_min,
                     double t_max,
                     bool abort_on_hit) const {
    HitResult rtr{};
    HitResult tmp{};
    double last_closest = t_max;

    for (const auto& obj : objects) {
        for (const auto& face : obj.faces) {
            Vec3 v0 = vertex_data[static_cast<size_t>(face.x - 1)];
            Vec3 v1 = vertex_data[static_cast<size_t>(face.y - 1)];
            Vec3 v2 = vertex_data[static_cast<size_t>(face.z - 1)];
            Triangle tri{v0, v1, v2};
            tmp = tri.hit(ray);
            tmp.point = ray.at(tmp.t);
            tmp.obj_id = obj.id;
            tmp.material_id = obj.material_id;
            if (tmp.is_hit) {
                if (tmp.t > t_min && tmp.t < last_closest) {
                    last_closest = tmp.t;
                    rtr = tmp;
                    if (abort_on_hit) {
                        return rtr;
                    }
                }
            }
        }
    }

    return rtr;
}

} // namespace XmlRaytracer
