#pragma once
#include "../resource_type_factory.h"

namespace VkRenderer::Resources
{
    class SemaphoreFactory : public VulkanResourceTypeFactory
    {
    public:
        Result<vk::raii::Semaphore, ErrorMessage> create(const vk::SemaphoreCreateInfo& desc) const;
    };
}