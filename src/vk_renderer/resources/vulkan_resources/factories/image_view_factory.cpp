#include "image_view_factory.h"

namespace VkRenderer::Resources
{
    Result<vk::raii::ImageView, ErrorMessage> ImageViewFactory::create(
        vk::ImageViewCreateInfo& desc) const
    {
        return castVkResult(getContext().device.createImageView(desc));
    }

    /* leaves only the following fields undefined:
        - `image`
    */
    vk::ImageViewCreateInfo ImageViewFactory::defaultDescription()
    {
        return vk::ImageViewCreateInfo {
            .viewType = vk::ImageViewType::e2D,
            .format = vk::Format::eR8G8B8A8Srgb,
            .subresourceRange = vk::ImageSubresourceRange {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
    }
}