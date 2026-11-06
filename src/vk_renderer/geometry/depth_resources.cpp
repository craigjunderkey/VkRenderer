#include "../vk_renderer.h"
#include "depth_resources.h"
#include "texture.h"

using VkRenderer::Resources::TextureDescription;

namespace VkRenderer::Geometry
{
    Result<vk::Format, ErrorMessage> findSupportedFormat(App& app, const std::vector<vk::Format>& candidates,
        vk::ImageTiling tiling, vk::FormatFeatureFlags features)
    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties properties = app.physicalDevice.getFormatProperties(format);
            bool linearMatch = tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features;
            bool optimalMatch = tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features;
            if (linearMatch || optimalMatch) return format;
        }

        return ErrorMessage({ .channel = "vulkan" }, "Failed to find supported depth format.");
    }
    
    Result<Unit, ErrorMessage> createDepthTexture(App& app)
    {
        // get format
        auto depthFormatResult = findSupportedFormat(
            app, 
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
        CHECK_KEEP(depthFormatResult);
        vk::Format depthFormat = depthFormatResult.take();

        // create texture
        TextureDescription desc = TextureDescription::screenTexture(app.swapchain.getExtent());
        desc.format = depthFormat;
        desc.usageFlags = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        desc.aspectFlags = vk::ImageAspectFlagBits::eDepth;

        auto texResult = Texture::fromDescription(app, desc);
        CHECK_KEEP(texResult);
        app.depthTexture = texResult.take();

        // set debug name
        auto vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)
            vkGetInstanceProcAddr(*app.instance, "vkSetDebugUtilsObjectNameEXT");

        VkDebugUtilsObjectNameInfoEXT objInfo {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = VK_OBJECT_TYPE_IMAGE,
            .objectHandle = (u64)(VkImage)(*app.depthTexture.image),
            .pObjectName = "Image_DepthRenderTarget"
        };

        CHECK(castVkResult(vkSetDebugUtilsObjectNameEXT(*app.device, &objInfo)));

        return Ok();
    }
}