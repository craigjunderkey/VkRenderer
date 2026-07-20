#include "../hello_triangle_application.h"
#include "depth_resources.h"
#include "texture.h"

namespace HelloTriangleApplication::Geometry
{
    Result<vk::Format, std::string> findSupportedFormat(App& app, const std::vector<vk::Format>& candidates,
        vk::ImageTiling tiling, vk::FormatFeatureFlags features)
    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties properties = app.physicalDevice.getFormatProperties(format);
            bool linearMatch = tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features;
            bool optimalMatch = tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features;
            if (linearMatch || optimalMatch) return format;
        }

        return Err<std::string>("Failed to find supported depth format.");
    }
    
    Result<Unit, std::string> createDepthTexture(App& app)
    {
        auto depthFormatResult = findSupportedFormat(
            app, 
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
        if (!depthFormatResult) return Err<std::string>(depthFormatResult.error());

        vk::Format depthFormat = depthFormatResult.take();

        TRY_ASSIGN(
            app.depthTexture,
            Texture::create(
                app,
                app.swapChainExtent.width,
                app.swapChainExtent.height,
                depthFormat,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                vk::ImageAspectFlagBits::eDepth,
                false
            )
        );

        return Ok();
    }
}