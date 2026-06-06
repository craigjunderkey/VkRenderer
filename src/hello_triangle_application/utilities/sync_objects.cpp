#include "../hello_triangle_application.h"
#include "sync_objects.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createSyncObjects(App& app)
    {
        // create semaphores
        TRY_ASSIGN(
            app.presentCompleteSemaphore,
            castVkResult(app.device.createSemaphore(vk::SemaphoreCreateInfo()))
        );
        TRY_ASSIGN(
            app.renderFinishedSemaphore,
            castVkResult(app.device.createSemaphore(vk::SemaphoreCreateInfo()))
        );

        // create fence
        TRY_ASSIGN(
            app.drawFence,
            castVkResult(app.device.createFence(vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled }))
        );
        
        return Ok();
    }
}