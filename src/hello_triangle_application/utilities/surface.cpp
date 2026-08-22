#include "../hello_triangle_application.h"
#include "surface.h"
#include <limits>
#include <algorithm>

using HelloTriangleApplication::App;

Result<vk::raii::SurfaceKHR, ErrorMessage> makeGlfwSurface(App& app)
{
    VkSurfaceKHR rawSurface;

    auto result = castVkResult(glfwCreateWindowSurface(*app.instance, *app.window, nullptr, &rawSurface));
    CHECK(result);
    
    vk::raii::SurfaceKHR surface = vk::raii::SurfaceKHR(app.instance, rawSurface);

    return surface;
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, ErrorMessage> createSurface(App& app)
    {
        auto surfaceResult = makeGlfwSurface(app);
        CHECK_KEEP(surfaceResult);
        app.surface = surfaceResult.take();
        return Ok();
    }
}