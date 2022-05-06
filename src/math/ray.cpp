#include "ray.hpp"

namespace XmlRaytracer {

Vec3 Ray::at(double t) const {
    Vec3 x = o + t * d;
    return x;
}

} // namespace XmlRaytracer
