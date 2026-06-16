#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace HelloTriangleApplication::Geometry
{
    struct Vertex
    {
        vec3 pos;
        vec3 color;
        vec2 texCoord;

        static vk::VertexInputBindingDescription bindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions();

        bool operator==(const Vertex& other) const
        {
            return this->pos == other.pos
                && this->color == other.color
                && this->texCoord == other.texCoord; 
        }
    };

    const std::vector<Vertex> VERTICES = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const std::vector<u32> INDICES = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };
}

namespace std
{
    template<> struct hash<HelloTriangleApplication::Geometry::Vertex>
    {
        size_t operator()(const HelloTriangleApplication::Geometry::Vertex& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}