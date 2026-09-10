#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"

namespace VkRenderer::Geometry
{
    struct VertexInfo
    {
        static vk::VertexInputBindingDescription bindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions();
    };
}