#include "../vk_renderer.h"
#include "color_resources.h"

using VkRenderer::Resources::TextureDescription;

namespace VkRenderer::Geometry
{
    Result<Unit, ErrorMessage> createColorTexture(App& app)
    {
        TextureDescription desc = TextureDescription::screenTexture(app.swapchain.getExtent());
        desc.format = app.swapchain.getFormat().format;
        desc.usageFlags = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
        desc.aspectFlags = vk::ImageAspectFlagBits::eColor;
        
        auto texResult = Texture::fromDescription(app, desc);
        CHECK_KEEP(texResult);
        app.colorTexture = texResult.take();

        // set debug name
        auto vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)
            vkGetInstanceProcAddr(*app.instance, "vkSetDebugUtilsObjectNameEXT");
        
        VkDebugUtilsObjectNameInfoEXT objInfo {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = VK_OBJECT_TYPE_IMAGE,
            .objectHandle = (u64)(VkImage)(*app.colorTexture.image),
            .pObjectName = "Image_ColorRenderTarget"
        };

        CHECK(castVkResult(vkSetDebugUtilsObjectNameEXT(*app.device, &objInfo)));

        return Ok();
    }
}