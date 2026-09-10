#include "../hello_triangle_application.h"
#include "sync_objects.h"

using VkRenderer::App;

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createSyncObjects(App& app)
    {
        assert(app.presentCompleteSemaphores.empty()
            && app.renderFinishedSemaphores.empty()
            && app.inFlightFences.empty());
            
        
        for (u32 i = 0; i < app.swapchain.getImages().size(); i++)
        {
            auto semaphoreResult = castVkResult(app.device.createSemaphore(vk::SemaphoreCreateInfo()));
            CHECK_KEEP(semaphoreResult);
            app.renderFinishedSemaphores.emplace_back(std::move(semaphoreResult.take()));
        }

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            // add semaphore
            auto semaphoreResult = castVkResult(app.device.createSemaphore(vk::SemaphoreCreateInfo()));
            CHECK_KEEP(semaphoreResult);
            app.presentCompleteSemaphores.emplace_back(std::move(semaphoreResult.take()));
            
            // add fence
            auto fenceResult = castVkResult(
                app.device.createFence(vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled })
            );
            CHECK_KEEP(fenceResult);
            app.inFlightFences.emplace_back(std::move(fenceResult.take()));
        }

        return Ok();
    }
}