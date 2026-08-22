#include "../hello_triangle_application.h"
#include "vulkan_debug_output.h"

namespace HelloTriangleApplication::Utilities
{
    VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    )
    {
        if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        {
            err({ .channel = "vulkan" }, "{}...  {}.", vk::to_string(messageTypes), pCallbackData->pMessage);
        }
        else if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            warn({ .channel = "vulkan" }, "{}... {}.", vk::to_string(messageTypes), pCallbackData->pMessage);
        }
        else if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
        {
            log({ .channel = "vulkan", .verbosity = 2 }, "{}... {}.", vk::to_string(messageTypes), pCallbackData->pMessage);
        }

        return vk::False;
    }

    Result<Unit, ErrorMessage> createDebugMessenger(App& app)
    {
        if (!VALIDATION_LAYERS_ENABLED)
            return Ok();
        
        vk::DebugUtilsMessengerCreateInfoEXT createInfo {
            .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
            .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            .pfnUserCallback = vkDebugCallback
        };
        
        // create debug messenger
        auto createResult = castVkResult(app.instance.createDebugUtilsMessengerEXT(createInfo));
        CHECK_KEEP(createResult);
        app.debugMessenger = createResult.take();

        return Ok();
    }
}