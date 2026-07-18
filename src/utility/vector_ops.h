#pragma once
#include "types.h"

namespace VectorOps
{
    inline f32 lengthSquared(vec2 vec)
    {
        return vec.x * vec.x + vec.y + vec.y;
    }

    inline vec2 normalize(vec2 vec)
    {
        f32 len = vec.length();
        if (len == 0)
            return vec2();
        else
            return vec / len;   
    }

    inline vec2 floor(vec2 vec)
    {
        return glm::floor(vec);
    }
}