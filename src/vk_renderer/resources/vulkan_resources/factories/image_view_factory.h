#pragma once
#include "../resource_type_factory.h"

namespace VkRenderer::Resources
{
    class ImageViewFactory : public VulkanResourceTypeFactory
    {
    public:
        Result<vk::raii::ImageView, ErrorMessage> create(const vk::ImageViewCreateInfo& desc) const;

        /* leaves only the following fields undefined:
            - `image`
        */
        static constexpr vk::ImageViewCreateInfo defaultDescription();
    };
}