#pragma once
#include "vulkan_common.h"

namespace VkRenderer { class App; }

namespace VkRenderer::Utilities
{
    VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );

    Result<Unit, ErrorMessage> createDebugMessenger(App& app);

    std::string_view viewVkMessageType(vk::DebugUtilsMessageTypeFlagsEXT type);
}
