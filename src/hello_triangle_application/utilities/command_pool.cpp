#include "../hello_triangle_application.h"
#include "command_pool.h"
#include <memory>

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createCommandPools(App& app)
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = app.graphicsQueue.familyIndex
        };

        auto graphicsCommandPool = castVkResult(app.device.createCommandPool(commandPoolCreateInfo));
        CHECK_KEEP(graphicsCommandPool);

        app.graphicsQueue.commandPool = std::make_shared<vk::raii::CommandPool>(graphicsCommandPool.take());

        return Ok();
    }
}