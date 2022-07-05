#pragma once

#include "utility.h"

namespace denoiser
{

utility::Image
denoise(
    const utility::Image& colorImage,
    const utility::Image& albedoImage,
    const utility::Image& normalImage);

}