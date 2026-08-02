#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication::Resources
{
    struct ImageData
    {
        u8* pixels; // ImageData owns the memory to which `pixels` points
        i32 width;
        i32 height;
        i32 numChannels;
    };
}