#include "../hello_triangle_application.h"
#include "image_views.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<vk::raii::ImageView, std::string> createImageView(App& app, const vk::Image& image,
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
    
    Result<Unit, std::string> createImageViews(App& app)
    {
        if (!app.swapChainImageViews.empty()) return Err<std::string>("Already swap chain image views present.");

        app.swapChainImageViews.reserve(app.swapChainImages.size());

        for (vk::Image& image : app.swapChainImages)
        {
            vk::raii::ImageView view = nullptr;
            TRY_ASSIGN(
                view,
                createImageView(app, image, app.swapChainFormat.format)
            );
            app.swapChainImageViews.emplace_back(std::move(view));
        }
        
        return Ok();
    }
}