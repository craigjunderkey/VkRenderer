#pragma once
#include "transform.h"

namespace HelloTriangleApplication::RenderLoop
{
    class RenderObject
    {
    public:
        StringId mesh;
        StringId material;
        Transform transform;
    };
}