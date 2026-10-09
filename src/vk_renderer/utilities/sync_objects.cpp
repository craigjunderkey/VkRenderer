#include "../vk_renderer.h"
#include "sync_objects.h"

using VkRenderer::App;

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createSyncObjects(App& app)
    {
        assert(app.presentCompleteSemaphores.empty()
            && app.renderFinishedSemaphores.empty()
            && app.renderFinishedFences.empty()
            && app.computeFinishedSemaphores.empty()
            && app.computeFinishedFences.empty()
        );   
        
        for (u32 i = 0; i < app.swapchain.getImages().size(); i++)
        {
            auto semaphoreResult = app.vkResFactory.get<Resources::SemaphoreFactory>().create({});
            CHECK_KEEP(semaphoreResult);
            app.renderFinishedSemaphores.emplace_back(std::move(semaphoreResult.take()));
        }

        // render sync objects
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            auto semaphoreResult = app.vkResFactory.get<Resources::SemaphoreFactory>().create({});
            CHECK_KEEP(semaphoreResult);
            app.presentCompleteSemaphores.emplace_back(std::move(semaphoreResult.take()));
            
            auto fenceResult = app.vkResFactory.get<Resources::FenceFactory>().create({ .startSignaled = true });
            CHECK_KEEP(fenceResult);
            app.renderFinishedFences.emplace_back(std::move(fenceResult.take()));
        }

        // compute sync objects
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            auto semaphoreResult = app.vkResFactory.get<Resources::SemaphoreFactory>().create({});
            CHECK_KEEP(semaphoreResult);
            app.computeFinishedSemaphores.emplace_back(semaphoreResult.take());

            auto fenceResult = app.vkResFactory.get<Resources::FenceFactory>().create({ .startSignaled = true});
            CHECK_KEEP(fenceResult);
            app.computeFinishedFences.emplace_back(fenceResult.take());
        }

        return Ok();
    }
}