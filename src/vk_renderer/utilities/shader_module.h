#pragma once
#include "common.h"
#include "vulkan_common.h"
#include "../resources/shader.h"

namespace VkRenderer { class App; }

namespace VkRenderer::Utilities
{
    Result<vk::raii::ShaderModule, ErrorMessage> createShaderModule(App& app, const Resources::Shader& shader);
}