#include "../vk_renderer.h"
#include "shader_module.h"
#include "vulkan_debug.h"

using VkRenderer::Resources::Shader;

namespace VkRenderer::Utilities
{
    Result<vk::raii::ShaderModule, ErrorMessage> createShaderModule(App& app, const Resources::Shader& shader)
    {
        vk::ShaderModuleCreateInfo shaderModuleCreateInfo {
            .codeSize = shader.code.size() * sizeof(char),
            .pCode = reinterpret_cast<const u32*>(shader.code.data())
        };

        auto createResult = castVkResult(app.device.createShaderModule(shaderModuleCreateInfo));
        CHECK_KEEP(createResult);

        vk::raii::ShaderModule shaderModule = createResult.take();

#ifndef NDEBUG
        std::string* debugName = new std::string();
        *debugName = std::format("Shader_{}", StringId::getString(shader.name));
        setDebugName(app, *shaderModule, debugName->c_str());
#endif

        return shaderModule;
    }
}