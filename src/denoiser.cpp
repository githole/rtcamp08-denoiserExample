#include "denoiser.h"

namespace denoiser
{
    
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

#pragma omp parallel for schedule(dynamic, 1)
    for (int iy{ 0 }; iy < colorImage.height; ++iy)
    {
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

                    // filter in tonemapped space
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