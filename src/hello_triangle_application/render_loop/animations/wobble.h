#pragma once
#include "../animation.h"

namespace HelloTriangleApplication::RenderLoop::Animations
{
    class Wobble : public Animation
    {
    private:
        f32 frequency_;
        f32 amplitude_;
        f32 yOrigin_;

    public:
        Wobble(f32 frequency, f32 amplitude, f32 yOrigin) : frequency_(frequency), amplitude_(amplitude), yOrigin_(yOrigin) {}

        AnimationResult animate(Trs in, f32 time, f32 delta)
        {   
            Trs out = in;
            out.t.y = this->yOrigin_ + this->amplitude_ * glm::cos(time * this->frequency_);
            
            return AnimationResult {
                .out = out,
                .newAnim = {}
            };
        }
    };
}