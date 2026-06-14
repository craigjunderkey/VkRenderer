#pragma once

#include <vulkan/vulkan.hpp>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define GLFW_INCLUDE_VULKAN

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vulkan/vk_enum_string_helper.h>
#include "common.h"

constexpr u64 DISABLE_TIMEOUT = UINT64_MAX;

inline std::string vkResultToString(VkResult result)
{
    return std::string("Vulkan ") + std::string(string_VkResult(result));
}

inline std::string vkResultToString(vk::Result result)
{
    return std::string("Vulkan ") + std::string(string_VkResult((VkResult)result));
}

template<typename T>
inline Result<T, std::string> castVkResult(vk::ResultValue<T> result)
{
    if (result.has_value())
    {
        return Result<T, std::string>(std::move(result.value));
    }
    else
    {
        return Err<std::string>(vkResultToString(result.result));
    }
} 