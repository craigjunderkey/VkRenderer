#include "../vk_renderer.h"
#include "../geometry/buffer.h"
#include "particle.h"
#include "../utilities/staged_copy.h"
#include "../resources/shader.h"

namespace VkRenderer::Particles
{
    using Geometry::FatBuffer;
    using Utilities::StagedCopy;

    Result<ParticleSystem, ErrorMessage> createParticleSystem(App& app, u32 numParticles)
    {
        ParticleSystem particleSystem = { .numParticles = numParticles };
        
        // create particle buffers
        auto bufferCreateResult = [&app, numParticles] -> Result<std::vector<FatBuffer>, ErrorMessage>
        {
            std::vector<FatBuffer> buffers {};
            for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                auto buffer = FatBuffer::create(
                    app,
                    sizeof(Particle) * numParticles,
                    vk::BufferUsageFlagBits::eTransferDst
                        | vk::BufferUsageFlagBits::eStorageBuffer
                        | vk::BufferUsageFlagBits::eVertexBuffer,
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
            
            f32 inc = 2.0 * glm::pi<f32>() / numParticles;
            f32 spawnRadius = 1.0f;
            for (u32 i = 0; i < numParticles; i++)
            {
                particles[i] = Particle();
                particles[i].posRadius.x = spawnRadius * glm::cos(i * inc);
                particles[i].posRadius.y = 2.0f;
                particles[i].posRadius.z = spawnRadius * glm::sin(i * inc);
            }

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

    Result<Utilities::FatPipeline, ErrorMessage> createParticleComputePipeline(App& app)
    {
        // create pipeline layout
        auto layoutResult = [&descriptorSet = app.descriptorSetLayout, &device = app.device]
            -> Result<std::shared_ptr<vk::raii::PipelineLayout>, ErrorMessage>
        {
            vk::PipelineLayoutCreateInfo layoutCreateInfo {
                .setLayoutCount = 1,
                .pSetLayouts = &*descriptorSet
            };
            auto layoutResult = castVkResult(device.createPipelineLayout(layoutCreateInfo));
            CHECK_KEEP(layoutResult);

            return std::make_shared<vk::raii::PipelineLayout>(layoutResult.take());
        }();
        CHECK_KEEP(layoutResult);
        std::shared_ptr<vk::raii::PipelineLayout> layout = layoutResult.take();

        // create shader module
        Resources::Shader& shader = app.resources.get<Resources::Shader>("particle");

        auto moduleResult = [&device = app.device, &shader] -> Result<vk::raii::ShaderModule, ErrorMessage>
        {
            vk::ShaderModuleCreateInfo moduleInfo {
                .codeSize = shader.code.size(),
                .pCode = reinterpret_cast<const u32*>(shader.code.data())
            };
            auto moduleResult = castVkResult(device.createShaderModule(moduleInfo));
            CHECK_KEEP(moduleResult);
            
            return moduleResult.take();
        }();
        CHECK_KEEP(moduleResult);
        vk::raii::ShaderModule shaderModule = moduleResult.take();

        // create compute shader pipeline
        vk::PipelineShaderStageCreateInfo stageInfo {
            .stage = vk::ShaderStageFlagBits::eCompute,
            .module = *shaderModule,
            .pName = shader.getEntryPoint(Resources::ShaderStage::Compute).functionName.c_str()
        };
        
        vk::ComputePipelineCreateInfo pipelineCreateInfo {
            .stage = stageInfo,
            .layout = **layout
        };

        auto pipelineResult = castVkResult(app.device.createComputePipeline(nullptr, pipelineCreateInfo));
        CHECK_KEEP(pipelineResult);
        vk::raii::Pipeline pipeline = pipelineResult.take();

        return Utilities::FatPipeline {
            .pipeline = std::move(pipeline),
            .layout = std::move(layout)
        };
    }

    std::array<vk::VertexInputAttributeDescription, 1> getParticleVertexDescription()
    {
        return std::array<vk::VertexInputAttributeDescription, 1> {
            vk::VertexInputAttributeDescription {
                .location = 0,
                .binding = 0,
                .format = vk::Format::eR32G32B32A32Sfloat,
                .offset = offsetof(Particle, posRadius)
            }
        };
    }
}