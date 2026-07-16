#pragma once
#include "../utilities/common.h"
#include "transform.h"

#include <memory>

namespace HelloTriangleApplication::RenderLoop
{
    struct AnimationResult;
    
    class Animation
    {
    public:
        virtual AnimationResult animate(const Transform& xform, f32 time, f32 delta) = 0;
        virtual ~Animation() {}
    };

    struct AnimationResult
    {
        Transform xform;
        Option<std::unique_ptr<Animation>> newAnim;
    };
}