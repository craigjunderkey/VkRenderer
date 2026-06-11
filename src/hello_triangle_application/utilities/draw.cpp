#include "../hello_triangle_application.h"
#include "draw.h"
#include "command_buffer.h"
#include "swap_chain.h"

using HelloTriangleApplication::App;

struct NextImageResult
{
    Option<u32> imageIndex;
    bool recreatedSwapChain;
};

NextImageResult nextImageInSwapChain(App& app)
{
    vk::ResultValue<u32> image =
        app.swapChain.acquireNextImage(
            DISABLE_TIMEOUT,
            *app.presentCompleteSemaphores[app.frameIndex],
            VK_NULL_HANDLE
        );
    
    vk::Result returnCode = image.result;
    if (returnCode == vk::Result::eSuccess || returnCode == vk::Result::eSuboptimalKHR)
    {
        return NextImageResult { .imageIndex = image.value };
    }
    else if (returnCode == vk::Result::eErrorOutOfDateKHR)
    {
        HelloTriangleApplication::Utilities::recreateSwapChain(app);
        return NextImageResult { .imageIndex = image.value, .recreatedSwapChain = true };
    }
    else
    {
        return NextImageResult();
    }
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> drawFrame(App& app)
    {
        // wait for draw fence to be signaled
        vk::Result fenceResult = app.device.waitForFences(*app.inFlightFences[app.frameIndex], true, DISABLE_TIMEOUT);
        if (fenceResult != vk::Result::eSuccess)
            return Err<std::string>("Failed to wait for fence.");
        
        // so, we now have that the previous frame is done being drawn to the screen
        // so, request the next image from the swap chain
        auto nextImage = nextImageInSwapChain(app);
        if (!nextImage.imageIndex)
            return Err<std::string>("Failed to fetch next image from the swap chain.");
        else if (nextImage.recreatedSwapChain)
            return Ok();
        
        u32 imageIndex = nextImage.imageIndex.value();

        // return the draw fence to being unsignaled, since we are now on track to submit work
        // that will lead to it being resignaled
        app.device.resetFences(*app.inFlightFences[app.frameIndex]);

        // since we have the index of the incoming image, prepare our command buffer for drawing to it
        app.graphicsQueue.commandBuffers[app.frameIndex].reset();
        recordCommandBuffer(app, imageIndex);

        // submit draw call
        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vk::SubmitInfo submitInfo {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*app.presentCompleteSemaphores[app.frameIndex],
            .pWaitDstStageMask = &waitDestinationStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &*app.graphicsQueue.commandBuffers[app.frameIndex],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &*app.renderFinishedSemaphores[imageIndex]
        };

        vk::Result submitResult = app.graphicsQueue.queue.submit(submitInfo, *app.inFlightFences[app.frameIndex]);
        if (submitResult != vk::Result::eSuccess)
            return Err<std::string>("Failed to submit render request.");
        
        // present the frame
        vk::PresentInfoKHR presentInfo = {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*app.renderFinishedSemaphores[imageIndex],
            .swapchainCount = 1,
            .pSwapchains = &*app.swapChain,
            .pImageIndices = &imageIndex
        };

        vk::Result presentResult = app.graphicsQueue.queue.presentKHR(presentInfo);
        bool shouldRecreateSwapChain = presentResult == vk::Result::eSuboptimalKHR
            || presentResult == vk::Result::eErrorOutOfDateKHR
            || app.window.status.framebufferResized
            || app.window.status.isMinimized;
        
        if (shouldRecreateSwapChain)
        {
            recreateSwapChain(app);
            app.window.status.framebufferResized = false;
        }
        
        // advance frame
        app.frameIndex = (app.frameIndex + 1) % MAX_FRAMES_IN_FLIGHT; 
        
        return Ok();
    }
}