#pragma once
#include <memory>
#include "vulkan_common.h"

namespace HelloTriangleApplication::Utilities
{
    struct FatQueue
    {
        vk::raii::Queue queue = nullptr;
        u32 familyIndex = 0;
        std::vector<vk::raii::CommandBuffer> commandBuffers = {};
        std::shared_ptr<vk::raii::CommandPool> commandPool = nullptr;

        vk::raii::Queue operator*() { return this->queue; }
    };
}