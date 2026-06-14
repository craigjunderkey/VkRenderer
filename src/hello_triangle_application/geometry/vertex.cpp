#include "vertex.h"

const auto vkFloat = vk::Format::eR32Sfloat;
const auto vkFloat2 = vk::Format::eR32G32Sfloat;
const auto vkFloat3 = vk::Format::eR32G32B32Sfloat;
const auto vkFloat4 = vk::Format::eR32G32B32A32Sfloat;

namespace HelloTriangleApplication::Geometry
{
    vk::VertexInputBindingDescription Vertex::bindingDescription()
    {
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = vk::VertexInputRate::eVertex
        };
    }

    std::array<vk::VertexInputAttributeDescription, 3> Vertex::attributeDescriptions()
    {
        return {
            vk::VertexInputAttributeDescription {
                .location = 0,
                .binding = 0,
                .format = vkFloat2,
                .offset = offsetof(Vertex, pos)
            },
            vk::VertexInputAttributeDescription {
                .location = 1,
                .binding = 0,
                .format = vkFloat3,
                .offset = offsetof(Vertex, color)
            },
            vk::VertexInputAttributeDescription {
                .location = 2,
                .binding = 0,
                .format = vkFloat2,
                .offset = offsetof(Vertex, texCoord)
            }
        };
    }
}