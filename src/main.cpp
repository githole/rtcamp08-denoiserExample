#include "utility.h"

namespace denoiser
{

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

float luminance(const utility::Color& color)
{
    // assume sRGB color space
    return
        color[0] * 0.2126f +
        color[1] * 0.7152f +
        color[2] * 0.0722f;
}

// Reinhard-like tonemapping
utility::Color
applySimpleTonemap(const utility::Color& color)
{
    const auto coeff{ 1.0f / (luminance(color) + 1.0f)};
    return {
        color[0] * coeff,
        color[1] * coeff,
        color[2] * coeff,
    };
}

utility::Color
applyInverseSimpleTonemap(const utility::Color& color)
{
    const auto coeff{ 1.0f / (luminance(color) * -1.0f + 1.0f) };
    return {
        color[0] * coeff,
        color[1] * coeff,
        color[2] * coeff,
    };
}

// true if succeeded.
utility::Image
denoise(
    const utility::Image& colorImage,
    const utility::Image& albedoImage,
    const utility::Image& normalImage)
{
    utility::Image denoisedImage = colorImage;

    // simple Bilateral filter

    constexpr int K{ 8 };
    constexpr float s0_2{ 16.0f };
    constexpr float s1_2{ 0.1f };
    constexpr float s2_2{ 0.01f };

    for (int iy{ 0 }; iy < colorImage.height; ++iy)
    {
#pragma omp parallel for schedule(dynamic, 1)
        for (int ix{ 0 }; ix < colorImage.width; ++ix)
        {
            const auto color{ colorImage.load(ix, iy) };
            const auto albedo{ albedoImage.load(ix, iy) };
            const auto normal{ normalImage.load(ix, iy) };

            double weightSum{ 0 };
            std::array<double, 3> colorSum{};

            // main kernel
            for (int oy{ -K }; oy <= K; ++oy)
            {
                for (int ox{ -K }; ox <= K; ++ox)
                {
                    const auto colorNeighbor{ colorImage.load(ix + ox, iy + oy) };
                    const auto albedoNeighbor{ albedoImage.load(ix + ox, iy + oy) };
                    const auto normalNeighbor{ normalImage.load(ix + ox, iy + oy) };

                    const float w0{
                        exp(-(ox * ox + oy * oy) / (2.0f * s0_2)) };
                    const float w1{
                        exp(-utility::lengthSquared(albedo, albedoNeighbor) / (2.0f * s1_2)) };
                    const float w2{
                        exp(-utility::lengthSquared(normal, normalNeighbor) / (2.0f * s2_2)) };

                    const float w{ w0 * w1 * w2 };
                    weightSum += w;

                    const auto tmpColor{ applySimpleTonemap(colorNeighbor) };
                    for (int ch{ 0 }; ch < 3; ++ch)
                    {
                        colorSum[ch] += w * tmpColor[ch];
                    }
                }
            }

            utility::Color finalColor;
            for (int ch{ 0 }; ch < 3; ++ch)
            {
                finalColor[ch] = static_cast<float>(colorSum[ch] / weightSum);
            }
            finalColor = applyInverseSimpleTonemap(finalColor);

            denoisedImage.store(ix, iy, finalColor);
        }
    }

    return denoisedImage;
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
    denoiser::decodeNormalImage(decodedNormalImage);

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