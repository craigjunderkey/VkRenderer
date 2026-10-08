#include <vector>

namespace VkRenderer::Geometry { class FatBuffer; }

namespace VkRenderer::Particles
{
    struct ParticleSystem
    {
        std::vector<Geometry::FatBuffer> buffers = {};
    };
}