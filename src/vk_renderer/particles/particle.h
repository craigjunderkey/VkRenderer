#include "../../utility/types.h"

namespace VkRenderer::Particles
{
    struct Particle
    {
        vec4 posRadius = { 0.0f, 0.0f, 0.0f, 1.0f };
        vec4 velocity = { 0.0f, 0.0f, 0.0f, 0.0f };
        u16 neighborCount = 0;
        f32 phase;
        f32 waterAmount = 0.0f;
        f32 temperature;
    };
}