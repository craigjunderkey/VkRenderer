#include "particle.h"
#include "../utilities/common.h"

namespace VkRenderer { class App; }
namespace VkRenderer::Geometry { class FatBuffer; }

namespace VkRenderer::Particles
{
    /// @brief Creates a ParticleSystem by allocating and initalizing buffers full of Particles.
    /// @param app the app (uses logical device and memory helper)
    /// @return either a ParticleSystem or an error due to one of: allocation
    Result<ParticleSystem, ErrorMessage> createParticleSystem(App& app, u32 numParticles);
}