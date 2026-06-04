#pragma once

#include <vulkan/vulkan.hpp>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define VULKAN_HPP_NO_EXCEPTIONS

#define GLFW_INCLUDE_VULKAN