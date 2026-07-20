#include "../hello_triangle_application.h"
#include "color_resources.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createColorTexture(App& app)
    {
        TRY_ASSIGN(
            app.colorTexture,
            Texture::create(
                app,
                app.swapChainExtent.width,
                app.swapChainExtent.height,
                app.swapChainFormat.format,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                vk::ImageAspectFlagBits::eColor,
                false,
                app.msaaSamples
            )
        );

        return Ok();
    }
}