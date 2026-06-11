#include "../hello_triangle_application.h"
#include "command_pool.h"
#include <memory>

using HelloTriangleApplication::App;

Result<Unit, std::string> createGraphicsCommandPool(App& app)
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

Result<Unit, std::string> createTransferCommandPool(App& app)
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = app.transferQueue.familyIndex
    };

    auto commandPool = castVkResult(app.device.createCommandPool(commandPoolCreateInfo));
    CHECK_KEEP(commandPool);

    app.transferQueue.commandPool = std::make_shared<vk::raii::CommandPool>(commandPool.take());

    return Ok();
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createCommandPools(App& app)
    {
        createGraphicsCommandPool(app);
        createTransferCommandPool(app);

        return Ok();
    }
}