#pragma once
#include "../animation.h"

namespace HelloTriangleApplication::RenderLoop::Animations
{
    class Spin : public Animation
    {
    private:
        f32 spinRate_;

    public:
        Spin(f32 spinRate) : spinRate_(spinRate) {}

        AnimationResult animate(const Transform& xform, f32 time, f32 delta) override
        {
            Transform out = xform;
            out.r.y = time * this->spinRate_;
            
            return AnimationResult {
                .xform = out,
                .newAnim = {}
            };
        }
    };
}