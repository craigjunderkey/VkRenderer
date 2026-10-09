#include "../vk_renderer.h"
#include "draw.h"
#include "command_buffer.h"
#include "swap_chain.h"
#include "../geometry/uniform_buffer.h"

using VkRenderer::App;

struct NextImageResult
{
    Option<u32> imageIndex;
    bool recreatedSwapChain;
};

NextImageResult nextImageInSwapChain(App& app)
{
    vk::ResultValue<u32> image =
        app.swapchain->acquireNextImage(
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
        VkRenderer::Utilities::recreateSwapChain(app);
        return NextImageResult { .imageIndex = image.value, .recreatedSwapChain = true };
    }
    else
    {
        return NextImageResult();
    }
}

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> drawFrame(App& app, RenderLoop::GameLoop& loop)
    {
        // wait for draw fence to be signaled
        vk::Result fenceResult = app.device.waitForFences(*app.renderFinishedFences[app.frameIndex], true, DISABLE_TIMEOUT);
        if (fenceResult != vk::Result::eSuccess)
            return ErrorMessage({ .channel = "vulkan" }, "Failed to wait for fence.");
        
        // save the render as a screenshot
        if (app.frameRecorder.hasCapturedFrame(app.frameIndex))
            CHECK(app.frameRecorder.save(app.frameIndex));

        // so, we now have that the previous frame is done being drawn to the screen
        // so, request the next image from the swap chain
        auto nextImage = nextImageInSwapChain(app);
        if (!nextImage.imageIndex)
            return ErrorMessage({ .channel = "vulkan" }, "Failed to fetch next image from the swap chain.");
        else if (nextImage.recreatedSwapChain)
            return Ok();
        
        u32 imageIndex = nextImage.imageIndex.value();

        // dispatch compute before rendering
        VkRenderer::Geometry::updatePerFrameUbo(app, app.frameIndex);
        CHECK(dispatchCompute(app));

        // return the draw fence to being unsignaled, since we are now on track to submit work
        // that will lead to it being resignaled
        app.device.resetFences(*app.renderFinishedFences[app.frameIndex]);

        // since we have the index of the incoming image, prepare our command buffer for drawing to it
        app.graphicsQueue.commandBuffers[app.frameIndex].reset();

        beginRender(app, imageIndex);
        loop.draw(app.graphicsQueue.commandBuffers[app.frameIndex]);
        endRender(app, imageIndex);

        // submit draw call
        std::array<vk::Semaphore, 2> waitSemaphores {
            *app.presentCompleteSemaphores[app.frameIndex],
            *app.computeFinishedSemaphores[app.frameIndex]
        };
        std::array<vk::PipelineStageFlags, 2> waitMasks {
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eVertexInput
        };
        
        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vk::SubmitInfo submitInfo {
            .waitSemaphoreCount = static_cast<u32>(waitSemaphores.size()),
            .pWaitSemaphores = waitSemaphores.data(),
            .pWaitDstStageMask = waitMasks.data(),
            .commandBufferCount = 1,
            .pCommandBuffers = &*app.graphicsQueue.commandBuffers[app.frameIndex],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &*app.renderFinishedSemaphores[imageIndex]
        };

        vk::Result submitResult = app.graphicsQueue.queue.submit(submitInfo, *app.renderFinishedFences[app.frameIndex]);
        if (submitResult != vk::Result::eSuccess)
            return ErrorMessage({ .channel = "vulkan" }, "Failed to submit render request.");

        // present the frame
        vk::PresentInfoKHR presentInfo = {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*app.renderFinishedSemaphores[imageIndex],
            .swapchainCount = 1,
            .pSwapchains = &**app.swapchain,
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