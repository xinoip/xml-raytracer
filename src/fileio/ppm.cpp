#include "ppm.hpp"

#include <cstdio>
#include <fmt/core.h>

#include <cassert>

namespace XmlRaytracer {

PixelData::PixelData(Color3 color) {
    r = static_cast<u8>(color.x);
    g = static_cast<u8>(color.y);
    b = static_cast<u8>(color.z);
}

size_t ImageData::loc(int x, int y) const {
    size_t index = static_cast<size_t>(y * width + x);
    return index;
}

PixelData& ImageData::operator()(int x, int y) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    size_t index = loc(x, y);
    PixelData& data = pixels[index];
    return data;
}

PixelData ImageData::operator()(int x, int y) const {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    size_t index = loc(x, y);
    PixelData data = pixels[index];
    return data;
}

bool ImageData::write_ppm(const std::string& path) {
    // TODO: maybe check for existing file so we don't overwrite any important
    // thing?
    //  FILE *fp = fopen(path.c_str(), "r");
    //  if (fp != NULL) {
    //      fmt::print("ppm_write_image: Couldn't create image file because a
    //      file "
    //                 "already exists at {}\n",
    //                 path);
    //      fclose(fp);
    //      return false;
    //  }

    FILE* fp = fopen(path.c_str(), "w+");
    if (fp == NULL) {
        fmt::print("ppm_write_image: Couldn't create image file.\n");
        return false;
    }

    fprintf(fp, "P3\n");
    fprintf(fp, "%d %d\n255\n", width, height);
    // for (auto& pixel : pixels) {
    //     fprintf(fp, "%u %u %u\n", pixel.r, pixel.g, pixel.b);
    // }

    reverse(pixels.begin(), pixels.end());

    for (int y = 0; y < height; y++) {
        for (int x = width - 1; x >= 0; x--) {
            auto& pixel = (*this)(x, y);
            fprintf(fp, "%u %u %u\n", pixel.r, pixel.g, pixel.b);
        }
    }

    // for (auto it = pixels.rbegin(); it != pixels.rend(); ++it) {
    //     auto& pixel = *it;
    //     fprintf(fp, "%u %u %u\n", pixel.r, pixel.g, pixel.b);
    // }

    fclose(fp);
    return true;
}

} // namespace XmlRaytracer
