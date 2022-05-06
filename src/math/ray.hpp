#pragma once

#include "vec3.hpp"

namespace XmlRaytracer {

// P(t) = o + td
struct Ray {
    Vec3 o, d;

    Vec3 at(double t) const;
};

} // namespace XmlRaytracer
