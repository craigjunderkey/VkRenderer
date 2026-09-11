#pragma once
#include "../resource_type_factory.h"

namespace VkRenderer::Resources
{
    class ImageViewFactory : public VulkanResourceTypeFactory
    {
    public:
        Result<vk::raii::ImageView, ErrorMessage> create(
            vk::ImageViewCreateInfo& desc) const;

        /* leaves only the following fields undefined:
            - `image`
        */
        static vk::ImageViewCreateInfo defaultDescription();
    };
}