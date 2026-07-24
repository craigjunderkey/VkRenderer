#pragma once
#include "common.h"
#include "vulkan_common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Utilities
{
    Result<std::vector<char>, std::string> readCode(const char* fileName);
    Result<vk::raii::ShaderModule, std::string> createShaderModule(App& app, const std::vector<char>& code);
    Result<vk::raii::ShaderModule, std::string> loadShaderModule(App& app, const char* fileName);
}