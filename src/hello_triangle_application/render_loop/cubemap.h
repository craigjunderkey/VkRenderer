#pragma once
#include "../utilities/common.h"
#include "../geometry/texture.h"

namespace VkRenderer::RenderLoop
{
    enum CubeSide
    {
        PosX,
        NegX,
        PosY,
        NegY,
        PosZ,
        NegZ
    };
    
    struct Cubemap
    {
        std::array<StringId, 6> sides;
        Geometry::Texture texArray;
    };
}