#pragma once

#include <array>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace utility
{
using Vec3 = std::array<float, 3>;
using Color = std::array<float, 3>;

template<typename T>
T clampValue(const T& x, const T& a, const T& b)
{
    if (x < a)
        return a;
    if (x > b)
        return b;
    return x;
}

template<typename V>
float dot(const V& a, const V& b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

template<typename V>
V normalizeUnsafe(const V& vec)
{
    const auto length2{ dot(vec, vec) };
    const auto invLength{ 1 / sqrt(length2) };

    return {
        vec[0] * invLength,
        vec[1] * invLength,
        vec[2] * invLength,
    };
}

template<typename V>
float lengthSquared(const V& a, const V& b)
{
    return
        (a[0] - b[0]) * (a[0] - b[0]) +
        (a[1] - b[1]) * (a[1] - b[1]) +
        (a[2] - b[2]) * (a[2] - b[2]);
}

struct Image
{
    std::vector<float> body;
    int width{};
    int height{};

    bool isValid() const
    {
        return !body.empty();
    }

    size_t clampedIndex(int x, int y) const
    {
        if (x <= 0)
            x = 0;
        if (x >= width)
            x = width - 1;
        if (y <= 0)
            y = 0;
        if (y >= height)
            y = height - 1;
        return ((size_t)x + (size_t)y * width) * 3;
    }

    Color load(int x, int y) const
    {
        const auto index{ clampedIndex(x, y) };
        return {
            body[index + 0],
            body[index + 1],
            body[index + 2],
        };
    }

    void store(int x, int y, const Color& color)
    {
        const auto index{ clampedIndex(x, y) };
        body[index + 0] = color[0];
        body[index + 1] = color[1];
        body[index + 2] = color[2];
    }
};

inline Image loadHDRImage(const char* filename)
{
    Image image;
    int component{};
    constexpr int reqComponent{ 3 };
    const float* data{ stbi_loadf(filename, &image.width, &image.height, &component, reqComponent) };
    if (!data)
    {
        return {};
    }
    const size_t elementCount{ (size_t)reqComponent * image.width * image.height };
    image.body.reserve(elementCount);
    image.body.insert(image.body.begin(), data, data + elementCount);
    stbi_image_free((void*)data);

    std::cout << "Loaded: " << filename << std::endl;
    return image;
}

// true if succeeded.
inline bool writeHDRImage(const char* filename, const Image& image)
{
    return stbi_write_hdr(filename, image.width, image.height, 3, image.body.data());
}

inline bool checkResolutionEquality(const Image& a, const Image& b)
{
    return a.width == b.width && a.height == b.height;
}

}