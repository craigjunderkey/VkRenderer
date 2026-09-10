#pragma once
#include "../animation.h"

namespace VkRenderer::RenderLoop::Animations
{
    class Spin : public Animation
    {
    private:
        f32 spinRate_;

    public:
        Spin(f32 spinRate) : spinRate_(spinRate) {}

        AnimationResult animate(Trs in, f32 time, f32 delta) override
        {
            Trs out = in;
            out.r.y = time * this->spinRate_;
            
            return AnimationResult {
                .out = out,
                .newAnim = {}
            };
        }
    };
}