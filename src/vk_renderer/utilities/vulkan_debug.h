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

    Result<Unit, ErrorMessage> setDebugName(App& app, vk::Image image, const char* name);
    Result<Unit, ErrorMessage> setDebugName(App& app, vk::ImageView imageView, const char* name);
    Result<Unit, ErrorMessage> setDebugName(App& app, vk::Buffer buffer, const char* name);
}
