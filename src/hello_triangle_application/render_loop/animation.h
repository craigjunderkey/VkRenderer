#pragma once
#include "../utilities/common.h"
#include "transform.h"

#include <memory>

namespace VkRenderer::RenderLoop
{
    struct AnimationResult;
    
    class Animation
    {
    public:
        virtual AnimationResult animate(const Trs in, f32 time, f32 delta) = 0;
        virtual ~Animation() {}
    };

    struct AnimationResult
    {
        Trs out;
        Option<std::unique_ptr<Animation>> newAnim;
    };
}