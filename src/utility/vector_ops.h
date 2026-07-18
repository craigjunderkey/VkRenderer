#pragma once
#include "types.h"

#include <limits>

namespace VectorOps
{
    constexpr f32 EPSILON = std::numeric_limits<f32>::epsilon();

    inline f32 lengthSquared(vec2 vec)
    {
        return vec.x * vec.x + vec.y * vec.y;
    }

    inline f32 lengthSquared(vec3 vec)
    {
        return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    }

    inline vec2 normalize(vec2 vec)
    {
        f32 len = glm::length(vec);
        if (len == 0)
            return vec2();
        else
            return vec / len;   
    }

    inline vec3 normalize(vec3 vec)
    {
        f32 len = glm::length(vec);
        if (len == 0)
            return vec3();
        else
            return vec / len;
    }

    inline vec4 normalize(vec4 vec)
    {
        f32 len = glm::length(vec);
        if (len == 0)
            return vec4();
        else
            return vec / len;
    }

    inline vec2 floor(vec2 vec)
    {
        return glm::floor(vec);
    }
}