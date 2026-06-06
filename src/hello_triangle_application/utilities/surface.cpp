#include "../hello_triangle_application.h"
#include "surface.h"

using HelloTriangleApplication::App;

Result<vk::raii::SurfaceKHR, std::string> makeGlfwSurface(App& app)
{
    VkSurfaceKHR rawSurface;
    VkResult result = glfwCreateWindowSurface(*app.instance, app.window, nullptr, &rawSurface);
    
    if (result != VkResult::VK_SUCCESS)
        return Err<std::string>(vkResultToString(result));
    
    vk::raii::SurfaceKHR surface = vk::raii::SurfaceKHR(app.instance, rawSurface);

    return surface;
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createSurface(App& app)
    {
        TRY_ASSIGN(
            app.surface,
            makeGlfwSurface(app)
        );

        return Ok();
    }
}