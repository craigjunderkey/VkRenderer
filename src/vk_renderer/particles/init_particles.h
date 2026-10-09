#include "particle.h"
#include "../utilities/common.h"
#include "../utilities/pipeline_builder.h"

namespace VkRenderer { class App; }
namespace VkRenderer::Geometry { class FatBuffer; }

namespace VkRenderer::Particles
{
    /// @brief Creates a ParticleSystem by allocating and initalizing buffers full of Particles.
    /// @param app the app (uses logical device and memory helper)
    Result<ParticleSystem, ErrorMessage> createParticleSystem(App& app, u32 numParticles);

    /// @brief Creates a FatPipeline handling the particle compute shader.
    Result<Utilities::FatPipeline, ErrorMessage> createParticleComputePipeline(App& app);
}