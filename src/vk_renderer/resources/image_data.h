#pragma once
#include "../utilities/common.h"

namespace VkRenderer::Resources
{
    struct ImageData
    {
        StringId name;
        u8* pixels;
        i32 width;
        i32 height;
        i32 numChannels;

        constexpr u32 size() const
        {
            return width * height * 4;
        }
    };
}