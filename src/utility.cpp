#include "utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <iostream>

namespace utility
{
    
Image loadHDRImage(const char* filename)
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
bool writeHDRImage(const char* filename, const Image& image)
{
    return stbi_write_hdr(filename, image.width, image.height, 3, image.body.data());
}

}