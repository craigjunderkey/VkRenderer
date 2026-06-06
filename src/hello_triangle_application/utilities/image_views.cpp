#include "../hello_triangle_application.h"
#include "image_views.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createImageViews(App& app)
    {
        if (app.swapChainImages.empty()) return Err<std::string>("No images in the swap chain.");

        vk::ImageViewCreateInfo imageViewCreateInfo {
            .viewType = vk::ImageViewType::e2D,
            .format = app.swapChainFormat.format,
            .components = vk::ComponentMapping {
                .r = vk::ComponentSwizzle::eIdentity,
                .g = vk::ComponentSwizzle::eIdentity,
                .b = vk::ComponentSwizzle::eIdentity,
                .a = vk::ComponentSwizzle::eIdentity,
            },
            .subresourceRange = vk::ImageSubresourceRange {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .levelCount = 1,
                .layerCount = 1
            },
        };

        for (vk::Image& image : app.swapChainImages)
        {
            imageViewCreateInfo.image = image;
            
            vk::raii::ImageView imageView = nullptr;
            TRY_ASSIGN(imageView, castVkResult(app.device.createImageView(imageViewCreateInfo)));
            app.swapChainImageViews.emplace_back(imageView);
        }
        
        return Ok();
    }
}