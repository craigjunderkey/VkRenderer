#include "../hello_triangle_application.h"
#include "command_pool.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createCommandPool(App& app)
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = app.queueIndex.value()
        };

        TRY_ASSIGN(
            app.commandPool,
            castVkResult(app.device.createCommandPool(commandPoolCreateInfo))
        );

        return Ok();
    }
}