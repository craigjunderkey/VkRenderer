#include "../vk_renderer.h"
#include "../geometry/buffer.h"
#include "particle.h"
#include "../utilities/staged_copy.h"

namespace VkRenderer::Particles
{
    using Geometry::FatBuffer;
    using Utilities::StagedCopy;

    Result<ParticleSystem, ErrorMessage> createParticleSystem(App& app, u32 numParticles)
    {
        ParticleSystem particleSystem = {};
        
        // create particle buffers
        auto bufferCreateResult = [&app, numParticles] -> Result<std::vector<FatBuffer>, ErrorMessage>
        {
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
        CHECK_KEEP(bufferCreateResult);
        particleSystem.buffers = bufferCreateResult.take();

        // initialize particle data
        std::vector<Particle> initialParticleData = [numParticles]
        {
            std::vector<Particle> particles(numParticles);
            return particles;
        }();

        // copy initial (CPU) particle data to all (GPU) particle buffers
        auto copyResult = [&app, &buffers = particleSystem.buffers, &initialParticleData] -> Result<Unit, ErrorMessage>
        {
            StagedCopy copy(app);
            copy.begin(initialParticleData.data(), initialParticleData.size() * sizeof(Particle));
            for (FatBuffer& buffer : buffers)
            {
                copy.addDst(*buffer);
            }
            CHECK(copy.end());
            return Ok();
        }();
        CHECK(copyResult);

        return particleSystem;
    }
}