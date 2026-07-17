#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication::RenderLoop
{
    class Camera
    {
    private:
        vec3 pos_;
        f32 xRot_;
        f32 yRot_;
    
    public:
        mat4 view() const;
    };
}