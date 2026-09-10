#pragma once
#include "../utilities/common.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace VkRenderer::Geometry
{
    struct Vertex
    {
        vec3 pos;
        vec3 normal;
        vec2 texCoord;

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
    template<> struct hash<VkRenderer::Geometry::Vertex>
    {
        size_t operator()(const VkRenderer::Geometry::Vertex& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}