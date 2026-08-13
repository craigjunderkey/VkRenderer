#pragma once
#include "common.h"
#include "vulkan_common.h"
#include "../resources/shader.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Utilities
{
    Result<vk::raii::ShaderModule, std::string> createShaderModule(App& app, const Resources::Shader& shader);
}