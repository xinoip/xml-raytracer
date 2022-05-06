#pragma once

#include <vector>
#include <string>
#include "dev.h"
#include "math/vec3.hpp"

namespace XmlRaytracer {

struct PixelData {
    u8 r, g, b;

    PixelData() = default;
    PixelData(Color3 color);
};

struct ImageData {
    int width, height;
    std::vector<PixelData> pixels;

    size_t loc(int x, int y) const;
    PixelData& operator()(int x, int y);
    PixelData operator()(int x, int y) const;

    bool write_ppm(const std::string& path);
};

} // namespace XmlRaytracer
