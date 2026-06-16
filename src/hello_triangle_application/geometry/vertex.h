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
        vec3 normal;
        vec2 texCoord;

        static vk::VertexInputBindingDescription bindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions();

        bool operator==(const Vertex& other) const
        {
            return this->pos == other.pos
                && this->normal == other.normal
                && this->texCoord == other.texCoord; 
        }
    };
}

namespace std
{
    template<> struct hash<HelloTriangleApplication::Geometry::Vertex>
    {
        size_t operator()(const HelloTriangleApplication::Geometry::Vertex& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}