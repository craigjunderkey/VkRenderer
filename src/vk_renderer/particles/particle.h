#include "../../utility/types.h"

namespace VkRenderer::Particles
{
    struct Particle
    {
        vec4 posRadius = { 0.0f, 0.0f, 0.0f, 1.0f };
        vec4 velocity = { 0.0f, 0.0f, 0.0f, 0.0f };
        u32 neighborCount = 0;
        
        // TODO(craigj): Find a default value for this in the paper. It's either
        // 1.0 or 0.0. Isn't this the ratio of water and ice?
        f32 phase;

        f32 waterAmount = 0.0f;
        
        // TODO(craigj): Find a default for this in the paper. I haven't read the
        // thermostuff, so I don't even really know what this does.
        f32 temperature;
    };
}