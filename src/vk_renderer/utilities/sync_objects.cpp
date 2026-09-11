#include "../vk_renderer.h"
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
            auto semaphoreResult = app.vkResFactory.get<Resources::SemaphoreFactory>().create({});
            CHECK_KEEP(semaphoreResult);
            app.renderFinishedSemaphores.emplace_back(std::move(semaphoreResult.take()));
        }

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            // add semaphore
            auto semaphoreResult = app.vkResFactory.get<Resources::SemaphoreFactory>().create({});
            CHECK_KEEP(semaphoreResult);
            app.presentCompleteSemaphores.emplace_back(std::move(semaphoreResult.take()));
            
            // add fence
            auto fenceResult = app.vkResFactory.get<Resources::FenceFactory>()
                .create({ .startSignaled = true });
            CHECK_KEEP(fenceResult);
            app.inFlightFences.emplace_back(std::move(fenceResult.take()));
        }

        return Ok();
    }
}