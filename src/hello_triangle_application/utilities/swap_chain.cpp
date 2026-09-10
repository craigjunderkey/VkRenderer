#include "../hello_triangle_application.h"
#include "swap_chain.h"
#include "image_views.h"
#include "../geometry/color_resources.h"
#include "../geometry/depth_resources.h"
#include "../render_loop/transform.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, ErrorMessage> createSwapChain(App& app)
    {
        // create window surface
        std::pair<u32, u32> windowDimensions = app.window.getFramebufferSize();
        
        SurfaceDescription surfaceDesc {
            .surface = *app.surface,
            .surfaceWidth = windowDimensions.first,
            .surfaceHeight = windowDimensions.second
        };

        // create swapchain
        FatSwapchainDescription swapDesc = FatSwapchainDescription::makeDefault();
        swapDesc.device = &app.device;
        swapDesc.physicalDevice = &app.physicalDevice;
        swapDesc.surfaceDesc = std::move(surfaceDesc);
        swapDesc.presentingQueueFamilyIndex = app.graphicsQueue.familyIndex;
        swapDesc.queueFamilyIndices = { app.graphicsQueue.familyIndex, app.transferQueue.familyIndex };

        auto createResult = FatSwapchain::create(std::move(swapDesc));
        CHECK_KEEP(createResult);
        app.swapchain = createResult.take();

        return Ok();
    }

    Result<Unit, ErrorMessage> recreateSwapChain(App& app)
    {
        while (app.window.status.isMinimized)
        {
            glfwWaitEvents();
        }

        app.device.waitIdle();

        std::pair<u32, u32> windowDimensions = app.window.getFramebufferSize();
        CHECK(app.swapchain.recreate(
            SurfaceDescription { .surfaceWidth = windowDimensions.first, .surfaceHeight = windowDimensions.second })
        );        
        CHECK(Geometry::createColorTexture(app));
        CHECK(Geometry::createDepthTexture(app));
        RenderLoop::createProjectionTransform(app);

        return Ok();
    }
}