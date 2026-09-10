#include "../hello_triangle_application.h"
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

        return Ok();
    }
}