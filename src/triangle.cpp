#include "triangle.hpp"

#include <cmath>
#include "dev.h"

namespace XmlRaytracer {

HitResult HitResult::no_hit() {
    HitResult rtr{};
    rtr.is_hit = false;
    return rtr;
}

Vec3 Triangle::normal() const {
    Vec3 v0v1 = v1 - v0;
    Vec3 v0v2 = v2 - v0;
    Vec3 rtr = cross(v0v1, v0v2);
    return rtr;
}

double Triangle::area() const {
    double area = normal().length() / 2;
    return area;
}

HitResult Triangle::hit(const Ray& ray) const {
    HitResult rtr = HitResult::no_hit();
    double t, u, v;

    Vec3 v0v1 = v1 - v0;
    Vec3 v0v2 = v2 - v0;
    Vec3 point_vector = cross(ray.d, v0v2);
    double determinant = dot(v0v1, point_vector);

#ifdef CULLING
    if (determinant < epsilon) {
        return rtr;
    }
#else
    if (fabs(determinant) < epsilon) {
        return rtr;
    }
#endif

    double inv_determinant = 1 / determinant;
    Vec3 tv = ray.o - v0;
    u = dot(tv, point_vector) * inv_determinant;
    if (u < 0 || u > 1) {
        return rtr;
    }

    Vec3 qv = cross(tv, v0v1);
    v = dot(ray.d, qv) * inv_determinant;
    if (v < 0 || u + v > 1) {
        return rtr;
    }

    t = dot(v0v2, qv) * inv_determinant;

    rtr.is_hit = true;
    rtr.point = {u, v, 1 - u - v};
    rtr.t = t;
    rtr.normal = unit_vector(normal()); // TODO: maybe convert to unit vector?

    return rtr;
}

} // namespace XmlRaytracer
