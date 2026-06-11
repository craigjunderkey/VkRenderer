#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"

namespace HelloTriangleApplication::Geometry
{
    struct Vertex
    {
        vec2 pos;
        vec3 color;

        static vk::VertexInputBindingDescription bindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions();
    };

    const std::vector<Vertex> VERTICES = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
}