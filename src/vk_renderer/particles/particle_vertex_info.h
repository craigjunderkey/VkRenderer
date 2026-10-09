#include "../utilities/vulkan_common.h"

namespace VkRenderer::Particles
{
    vk::VertexInputBindingDescription getParticleVertexBinding();
    std::array<vk::VertexInputAttributeDescription, 1> getParticleVertexAttributes();
}