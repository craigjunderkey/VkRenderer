#pragma once
#include <string>
#include <vulkan/vulkan_to_string.hpp>
#include "common.h"

constexpr u64 DISABLE_TIMEOUT = UINT64_MAX;

inline std::string vkResultToString(VkResult result)
{
    return std::string("Vulkan ") + std::to_string(result);
}

inline std::string vkResultToString(vk::Result result)
{
    return vkResultToString(static_cast<VkResult>(result));
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