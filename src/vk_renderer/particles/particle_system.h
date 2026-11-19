#include <vector>
#include "../../utility/types.h"

namespace VkRenderer::Geometry { class FatBuffer; }

namespace VkRenderer::Particles
{
    struct ParticleSystem
    {
        std::vector<Geometry::FatBuffer> buffers = {};
        u32 numParticles = 0;
    };

    struct ParticleSystemPushConstants
    {
        mat4 viewProjection = {};
    };
}