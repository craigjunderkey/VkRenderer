#include "../vk_renderer.h"
#include "vulkan_debug_output.h"

namespace VkRenderer::Utilities
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
            err({ .channel = "vulkan" }, "<{}>  {}", viewVkMessageType(messageTypes), pCallbackData->pMessage);
        }
        else if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            warn({ .channel = "vulkan", .verbosity = 1 }, "<{}>  {}", viewVkMessageType(messageTypes), pCallbackData->pMessage);
        }
        else
        {
            log({ .channel = "vulkan", .verbosity = 2 }, "<{}>  {}", viewVkMessageType(messageTypes), pCallbackData->pMessage);
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
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
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

    std::string_view viewVkMessageType(vk::DebugUtilsMessageTypeFlagsEXT type)
    {
        // message type strings
        constexpr std::string_view delimiter = " | ";
        constexpr std::string_view general = "General";
        constexpr std::string_view validation = "Validation";
        constexpr std::string_view performance = "Performance";
        constexpr std::string_view deviceAddressBinding = "DeviceAddressBinding";
        
        constexpr u32 maxSize = general.size() + delimiter.size() + validation.size() + delimiter.size()
            + performance.size() + delimiter.size() + performance.size() + delimiter.size() + deviceAddressBinding.size();

        // build message string
        static std::array<char, maxSize> msgString;
        auto pos = msgString.begin();

        bool isFirst = true;
        if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
        {
            pos = std::copy(general.begin(), general.end(), pos);
            isFirst = false;
        }
        if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
        {
            if (!isFirst)
            {
                pos = std::copy(delimiter.begin(), delimiter.end(), pos);
            }
            pos = std::copy(validation.begin(), validation.end(), pos);
            isFirst = false;
        }
        if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
        {
            if (!isFirst)
            {
                pos = std::copy(delimiter.begin(), delimiter.end(), pos);
            }
            pos = std::copy(performance.begin(), performance.end(), pos);
            isFirst = false;
        }
        if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding)
        {
            if (!isFirst)
            {
                pos = std::copy(delimiter.begin(), delimiter.end(), pos);
            }
            pos = std::copy(deviceAddressBinding.begin(), deviceAddressBinding.end(), pos);
            isFirst = false;
        }

        return std::string_view(msgString.data(), pos - msgString.begin());
    }
}