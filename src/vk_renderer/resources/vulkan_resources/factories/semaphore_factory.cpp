#include "semaphore_factory.h"

namespace VkRenderer::Resources
{
    Result<vk::raii::Semaphore, ErrorMessage> SemaphoreFactory::create(
        const vk::SemaphoreCreateInfo& desc) const
    {
        return castVkResult(getContext().device.createSemaphore(desc));
    }
}