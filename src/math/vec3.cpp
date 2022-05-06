#include "vec3.hpp"

#include <cassert>
#include <cmath>

namespace XmlRaytracer {

Vec3 Vec3::operator-() const {
    return {-x, -y, -z};
}

double Vec3::operator[](int i) const {
    if (i == 0)
        return x;
    if (i == 1)
        return y;
    if (i == 2)
        return z;

    assert(false);
}

double& Vec3::operator[](int i) {
    if (i == 0)
        return x;
    if (i == 1)
        return y;
    if (i == 2)
        return z;

    assert(false);
}

Vec3& Vec3::operator+=(const Vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vec3& Vec3::operator*=(const double t) {
    x *= t;
    y *= t;
    z *= t;
    return *this;
}

Vec3& Vec3::operator/=(const double t) {
    return *this *= 1 / t;
}

double Vec3::length() const {
    return sqrt(x * x + y * y + z * z);
}

Vec3 operator+(const Vec3& u, const Vec3& v) {
    return {u.x + v.x, u.y + v.y, u.z + v.z};
}

Vec3 operator-(const Vec3& u, const Vec3& v) {
    return {u.x - v.x, u.y - v.y, u.z - v.z};
}

Vec3 operator*(const Vec3& u, const Vec3& v) {
    return {u.x * v.x, u.y * v.y, u.z * v.z};
}

Vec3 operator*(double t, const Vec3& v) {
    return {t * v.x, t * v.y, t * v.z};
}

Vec3 operator*(const Vec3& v, double t) {
    return t * v;
}

Vec3 operator/(Vec3 v, double t) {
    return (1 / t) * v;
}

double dot(const Vec3& u, const Vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vec3 cross(const Vec3& u, const Vec3& v) {
    return {
        u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

Vec3 unit_vector(Vec3 v) {
    return v / v.length();
}

double cos_between(const Vec3& u, const Vec3& v) {
    double rtr = dot(u, v) / (u.length() * v.length());
    return rtr;
}

} // namespace XmlRaytracer
