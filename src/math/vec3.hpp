#pragma once

#include "dev.h"

namespace XmlRaytracer {

struct Vec3 {
    double x, y, z;

    double operator[](int i) const;
    double& operator[](int i);

    Vec3 operator-() const;
    Vec3& operator+=(const Vec3& v);
    Vec3& operator*=(const double t);
    Vec3& operator/=(const double t);

    double length() const;
};

using Point3 = Vec3;
using Color3 = Vec3;

Vec3 operator+(const Vec3& u, const Vec3& v);
Vec3 operator-(const Vec3& u, const Vec3& v);
Vec3 operator*(const Vec3& u, const Vec3& v);
Vec3 operator*(double t, const Vec3& v);
Vec3 operator*(const Vec3& v, double t);
Vec3 operator/(Vec3 v, double t);

double dot(const Vec3& u, const Vec3& v);
Vec3 cross(const Vec3& u, const Vec3& v);
Vec3 unit_vector(Vec3 v);

double cos_between(const Vec3& u, const Vec3& v);

} // namespace XmlRaytracer