#pragma once

#include "math/vec3.hpp"
#include "math/ray.hpp"

namespace XmlRaytracer {

struct HitResult {
    bool is_hit;
    Vec3 point;
    Vec3 normal;
    double t;
    int obj_id;
    int material_id;

    static HitResult no_hit();
};

struct Triangle {
    Vec3 v0, v1, v2;

    double area() const;
    Vec3 normal() const;
    HitResult hit(const Ray& ray) const;
};

} // namespace XmlRaytracer
