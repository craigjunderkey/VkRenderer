#include "../vk_renderer.h"
#include "color_resources.h"
#include "../utilities/vulkan_debug.h"

using VkRenderer::Resources::TextureDescription;

namespace VkRenderer::Geometry
{
    using namespace Utilities;

    Result<Unit, ErrorMessage> createColorTexture(App& app)
    {
        TextureDescription desc = TextureDescription::screenTexture(app.swapchain.getExtent());
        desc.format = app.swapchain.getFormat().format;
        desc.usageFlags = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
        desc.aspectFlags = vk::ImageAspectFlagBits::eColor;
        
        auto texResult = Texture::fromDescription(app, desc);
        CHECK_KEEP(texResult);
        app.colorTexture = texResult.take();

        CHECK(setDebugName(app, *app.colorTexture.image, "Image_ColorRenderTarget"));
        CHECK(setDebugName(app, *app.colorTexture.view, "ImageView_ColorRenderTarget"));

        return Ok();
    }
}