#include "particle_vertex_info.h"
#include "particle.h"

namespace VkRenderer::Particles
{
    vk::VertexInputBindingDescription getParticleVertexBinding()
    {
        return vk::VertexInputBindingDescription {
            .binding = 0,
            .stride = sizeof(Particle),
            .inputRate = vk::VertexInputRate::eVertex
        };
    }

    std::array<vk::VertexInputAttributeDescription, 1> getParticleVertexAttributes()
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