#include "../hello_triangle_application.h"
#include "color_resources.h"

using HelloTriangleApplication::Resources::TextureDescription;

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createColorTexture(App& app)
    {
        TextureDescription desc = TextureDescription::screenTexture(app.swapChainExtent);
        desc.format = app.swapChainFormat.format;
        desc.usageFlags = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
        desc.aspectFlags = vk::ImageAspectFlagBits::eColor;
        
        auto texResult = Texture::fromDescription(app, desc);
        CHECK_KEEP(texResult);
        app.colorTexture = texResult.take();
        
        // TRY_ASSIGN(
        //     app.colorTexture,
        //     Texture::create(
        //         app,
        //         app.swapChainExtent.width,
        //         app.swapChainExtent.height,
        //         app.swapChainFormat.format,
        //         vk::ImageTiling::eOptimal,
        //         vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
        //         vk::MemoryPropertyFlagBits::eDeviceLocal,
        //         vk::ImageAspectFlagBits::eColor,
        //         false,
        //         app.msaaSamples
        //     )
        // );

        return Ok();
    }
}