#include "../hello_triangle_application.h"
#include "sync_objects.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createSyncObjects(App& app)
    {
        assert(app.presentCompleteSemaphores.empty()
            && app.renderFinishedSemaphores.empty()
            && app.inFlightFences.empty());
            
        
        for (u32 i = 0; i < app.swapChainImages.size(); i++)
        {
            vk::raii::Semaphore semaphore = nullptr;
            TRY_ASSIGN(
                semaphore,
                castVkResult(app.device.createSemaphore(vk::SemaphoreCreateInfo()))
            );
            app.renderFinishedSemaphores.emplace_back(std::move(semaphore));
        }

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            // add semaphore
            vk::raii::Semaphore semaphore = nullptr;
            TRY_ASSIGN(
                semaphore,
                castVkResult(app.device.createSemaphore(vk::SemaphoreCreateInfo()))
            );
            app.presentCompleteSemaphores.emplace_back(std::move(semaphore));
            
            // add fence
            vk::raii::Fence fence = nullptr;
            TRY_ASSIGN(
                fence,
                castVkResult(app.device.createFence(vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled }))
            );
            app.inFlightFences.emplace_back(std::move(fence));
        }

        return Ok();
    }
}