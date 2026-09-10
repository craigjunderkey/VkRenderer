#include "../hello_triangle_application.h"
#include "command_pool.h"
#include <memory>

using VkRenderer::App;

Result<Unit, ErrorMessage> createGraphicsCommandPool(App& app)
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = app.graphicsQueue.familyIndex
    };

    auto commandPool = castVkResult(app.device.createCommandPool(commandPoolCreateInfo));
    CHECK_KEEP(commandPool);

    app.graphicsQueue.commandPool = std::make_shared<vk::raii::CommandPool>(commandPool.take());

    return Ok();
}

Result<Unit, ErrorMessage> createTransferCommandPool(App& app)
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo {
        .flags = vk::CommandPoolCreateFlagBits::eTransient,
        .queueFamilyIndex = app.transferQueue.familyIndex
    };

    auto commandPool = castVkResult(app.device.createCommandPool(commandPoolCreateInfo));
    CHECK_KEEP(commandPool);

    app.transferQueue.commandPool = std::make_shared<vk::raii::CommandPool>(commandPool.take());

    return Ok();
}

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createCommandPools(App& app)
    {
        createGraphicsCommandPool(app);
        createTransferCommandPool(app);

        return Ok();
    }
}