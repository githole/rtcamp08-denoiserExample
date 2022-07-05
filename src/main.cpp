#include "utility.h"
#include "denoiser.h"

#include <iostream>

void decodeNormalImage(utility::Image& normalImage)
{
    for (int iy{ 0 }; iy < normalImage.height; ++iy)
    {
        for (int ix{ 0 }; ix < normalImage.width; ++ix)
        {
            auto value{ normalImage.load(ix, iy) };
            for (auto& v : value)
            {
                v = utility::clampValue(2 * v - 1, -1.0f, 1.0f);
            }
            value = utility::normalizeUnsafe(value);

            normalImage.store(ix, iy, value);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc <= 4)
    {
        std::cout << "Invalid arguments." << std::endl;
        return 1;
    }

    const char* inputColor{ argv[1] };
    const char* inputAlbedo{ argv[2] };
    const char* inputNormal{ argv[3] };
    const char* outputColor{ argv[4] };

    const utility::Image inputColorImage{ utility::loadHDRImage(inputColor) };
    const utility::Image inputAlbedoImage{ utility::loadHDRImage(inputAlbedo) };
    const utility::Image inputNormalImage{ utility::loadHDRImage(inputNormal) };

    if (!inputColorImage.isValid() ||
        !inputAlbedoImage.isValid() ||
        !inputNormalImage.isValid())
    {
        std::cout << "Failed: load hdr file." << std::endl;
        return 1;
    }

    if (!utility::checkResolutionEquality(inputColorImage, inputAlbedoImage) ||
        !utility::checkResolutionEquality(inputColorImage, inputNormalImage))
    {
        std::cout << "Failed: hdr file resoultions are not matched." << std::endl;
        return 1;
    }

    utility::Image decodedNormalImage{ inputNormalImage };
    decodeNormalImage(decodedNormalImage);

    const auto denoisedImage{ denoiser::denoise(
        inputColorImage,
        inputAlbedoImage,
        decodedNormalImage
    ) };

    if (!utility::writeHDRImage(outputColor, denoisedImage))
    {
        std::cout << "Failed: save hdr file (" << outputColor << ")" << std::endl;
        return 1;
    }

    return 0;
}