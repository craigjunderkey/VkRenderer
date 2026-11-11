#include "../vk_renderer.h"
#include "vulkan_debug.h"

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

        vk::DebugUtilsMessengerCreateInfoEXT createInfo{
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

    static Result<Unit, ErrorMessage> setDebugName(App& app, VkObjectType objType, u64 handle, const char* name)
    {
        auto vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)
            vkGetInstanceProcAddr(*app.instance, "vkSetDebugUtilsObjectNameEXT");

        VkDebugUtilsObjectNameInfoEXT objInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = objType,
            .objectHandle = handle,
            .pObjectName = name
        };

        return castVkResult(vkSetDebugUtilsObjectNameEXT(*app.device, &objInfo));
    }

    Result<Unit, ErrorMessage> setDebugName(App& app, vk::Image image, const char* name)
    {
        return setDebugName(app, VK_OBJECT_TYPE_IMAGE, (u64)(VkImage)image, name);
    }

    Result<Unit, ErrorMessage> setDebugName(App& app, vk::ImageView imageView, const char* name)
    {
        return setDebugName(app, VK_OBJECT_TYPE_IMAGE_VIEW, (u64)(VkImageView)imageView, name);
    }

    Result<Unit, ErrorMessage> setDebugName(App& app, vk::Buffer buffer, const char* name)
    {
        return setDebugName(app, VK_OBJECT_TYPE_BUFFER, (u64)(VkBuffer)buffer, name);
    }

    Result<Unit, ErrorMessage> setDebugName(App& app, vk::ShaderModule shader, const char* name)
    {
        return setDebugName(app, VK_OBJECT_TYPE_SHADER_MODULE, (u64)(VkShaderModule)shader, name);
    }

    Result<Unit, ErrorMessage> setDebugName(App& app, vk::DescriptorSetLayout layout, const char* name)
    {
        return setDebugName(app, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (u64)(VkDescriptorSetLayout)layout, name);
    }

    Result<Unit, ErrorMessage> setDebugName(App& app, vk::DescriptorSet set, const char* name)
    {
        return setDebugName(app, VK_OBJECT_TYPE_DESCRIPTOR_SET, (u64)(VkDescriptorSet)set, name);
    }

    Result<Unit, ErrorMessage> setDebugName(App& app, vk::DescriptorPool pool, const char* name)
    {
        return setDebugName(app, VK_OBJECT_TYPE_DESCRIPTOR_POOL, (u64)(VkDescriptorPool)pool, name);
    }
}