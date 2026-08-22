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
inline Result<T, ErrorMessage> castVkResult(vk::ResultValue<T> result)
{
    if (result.has_value())
    {
        return Result<T, ErrorMessage>(std::move(result.value));
    }
    else
    {
        return ErrorMessage({ .channel = "vulkan" }, "From Vulkan: {}.", vkResultToString(result.result));
    }
} 

inline Result<Unit, ErrorMessage> castVkResult(vk::Result result)
{
    if (result == vk::Result::eSuccess)
    {
        return Ok();
    }
    else
    {
        return ErrorMessage({ .channel = "vulkan" }, "From Vulkan: {}.", vkResultToString(result));
    }
}

inline Result<Unit, ErrorMessage> castVkResult(VkResult result)
{
    if (result == VkResult::VK_SUCCESS)
    {
        return Ok();
    }
    else
    {
        return ErrorMessage({ .channel = "vulkan" }, "From Vulkan: {}.", vkResultToString(result));
    }
}