#pragma once
#include "../resource_type_factory.h"

namespace VkRenderer::Resources
{
    struct FenceDescription
    {
        bool startSignaled;

        constexpr vk::FenceCreateInfo toInfo() const;
    };
    
    class FenceFactory : public VulkanResourceTypeFactory
    {
    public:
        Result<vk::raii::Fence, ErrorMessage> create(const vk::FenceCreateInfo& desc) const;
        Result<vk::raii::Fence, ErrorMessage> create(const FenceDescription& desc) const;
    };
}