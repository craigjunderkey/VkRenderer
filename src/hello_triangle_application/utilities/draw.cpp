#include "../hello_triangle_application.h"
#include "draw.h"
#include "command_buffer.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> drawFrame(App& app)
    {
        // wait for draw fence to be signaled
        auto fenceResult = app.device.waitForFences(*app.drawFence, true, DISABLE_TIMEOUT);
        if (fenceResult != vk::Result::eSuccess)
            return Err<std::string>("Failed to wait for fence.");
        
        // return the draw fence to being unsignaled
        app.device.resetFences(*app.drawFence);

        // so, we now have that the previous frame is done being drawn to the screen
        // so, request the next image from the swap chain
        u32 imageIndex;
        TRY_ASSIGN(
            imageIndex,
            castVkResult(app.swapChain.acquireNextImage(DISABLE_TIMEOUT, *app.presentCompleteSemaphore, VK_NULL_HANDLE))
        );

        // since we have the index of the incoming image, prepare our command buffer for drawing to it
        recordCommandBuffer(app, imageIndex);

        // submit draw call
        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vk::SubmitInfo submitInfo {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*app.presentCompleteSemaphore,
            .pWaitDstStageMask = &waitDestinationStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &*app.commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &*app.renderFinishedSemaphore
        };

        auto submitResult = app.queue.submit(submitInfo, *app.drawFence);
        if (submitResult != vk::Result::eSuccess)
            return Err<std::string>("Failed to submit render request.");
        
        // present the frame
        vk::PresentInfoKHR presentInfo = {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*app.renderFinishedSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &*app.swapChain,
            .pImageIndices = &imageIndex
        };

        auto presentResult = app.queue.presentKHR(presentInfo);
        if (presentResult != vk::Result::eSuccess)
            return Err<std::string>("Failed to submit present request.");
        
        return Ok();
    }
}