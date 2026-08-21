#pragma once

#define VULKAN_HPP_NO_STRUCT_SETTERS
#define VULKAN_HPP_NAMESPACE vk
#define VULKAN_HPP_EXPORT
#define VULKAN_HPP_CONSTEXPR constexpr

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define GLFW_INCLUDE_VULKAN

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "vulkan_error_checking.h"