#pragma once
#include "../utilities/common.h"

namespace VkRenderer::Resources
{
    struct ImageData
    {
        StringId name;
        u8* pixels; // ImageData owns the memory to which `pixels` points
        i32 width;
        i32 height;
        i32 numChannels;

        constexpr u32 size() const
        {
            return width * height * 4;
        }
    };
}