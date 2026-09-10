#include "../hello_triangle_application.h"
#include "image_views.h"

using VkRenderer::App;

namespace VkRenderer::Utilities
{
    Result<vk::raii::ImageView, ErrorMessage> createImageView(App& app, const vk::Image& image,
        vk::Format format, vk::ImageAspectFlags aspectFlags, u32 mipLevels)
    {
        vk::ImageViewCreateInfo viewInfo {
            .image = image,
            .viewType = vk::ImageViewType::e2D,
            .format = format,
            .subresourceRange {
                .aspectMask = aspectFlags,
                .baseMipLevel = 0,
                .levelCount = mipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        return castVkResult(app.device.createImageView(viewInfo));
    }
}