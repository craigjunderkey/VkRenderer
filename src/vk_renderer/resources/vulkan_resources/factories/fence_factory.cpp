#include "fence_factory.h"

namespace VkRenderer::Resources
{
    constexpr vk::FenceCreateInfo FenceDescription::toInfo() const
    {
        vk::FenceCreateInfo info;
        if (this->startSignaled)
            info.flags |= vk::FenceCreateFlagBits::eSignaled;
        return info;
    }
    
    Result<vk::raii::Fence, ErrorMessage> FenceFactory::create(const vk::FenceCreateInfo& desc) const
    {
        return castVkResult(this->getContext().device.createFence(desc));
    }

    Result<vk::raii::Fence, ErrorMessage> FenceFactory::create(const FenceDescription& desc) const
    {
        return this->create(desc.toInfo());
    }
}