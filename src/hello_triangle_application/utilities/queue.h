#pragma once
#include <memory>
#include "vulkan_common.h"

namespace HelloTriangleApplication::Utilities
{
    struct FatQueue
    {
        vk::raii::Queue queue = nullptr;
        u32 familyIndex = 0;
        std::shared_ptr<vk::raii::CommandPool> commandPool = nullptr;
        std::vector<vk::raii::CommandBuffer> commandBuffers = {};

        vk::raii::Queue& operator*() { return this->queue; }
    };
}