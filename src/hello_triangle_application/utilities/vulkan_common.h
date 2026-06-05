#pragma once

#include <vulkan/vulkan.hpp>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define GLFW_INCLUDE_VULKAN

#include <vulkan/vk_enum_string_helper.h>
#include "common.h"

template<typename T>
inline Result<T, std::string> castVkResult(vk::ResultValue<T> result)
{
    if (result.has_value())
    {
        return Result<T, std::string>(std::move(result.value));
    }
    else
    {
        return Err<std::string>(std::string("Vulkan ") + std::string(string_VkResult((VkResult)result.result)));
    }
} 