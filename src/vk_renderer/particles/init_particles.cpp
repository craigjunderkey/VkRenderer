#include "../vk_renderer.h"
#include "../geometry/buffer.h"
#include "particle.h"

namespace VkRenderer::Particles
{
    using Geometry::FatBuffer;

    Result<ParticleSystem, ErrorMessage> createParticleSystem(App& app, u32 numParticles)
    {
        ParticleSystem particleSystem = {};
        
        // create particle buffers
        auto bufferResult = [&app, numParticles] -> Result<std::vector<FatBuffer>, ErrorMessage> {
            std::vector<FatBuffer> buffers {};
            for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                
                auto buffer = FatBuffer::create(
                    app,
                    sizeof(Particle) * numParticles,
                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eStorageBuffer,
                    vk::MemoryPropertyFlagBits::eDeviceLocal
                );
                CHECK_KEEP(buffer);
                buffers.emplace_back(buffer.take());
            }
            return buffers;
        }();
        CHECK_KEEP(bufferResult);
        particleSystem.buffers = bufferResult.take();

        return particleSystem;
    }
}