#include "../hello_triangle_application.h"
#include "shader_module.h"

using HelloTriangleApplication::Resources::Shader;

namespace HelloTriangleApplication::Utilities
{
    Result<vk::raii::ShaderModule, ErrorMessage> createShaderModule(App& app, const Resources::Shader& shader)
    {
        vk::ShaderModuleCreateInfo shaderModuleCreateInfo {
            .codeSize = shader.code.size() * sizeof(char),
            .pCode = reinterpret_cast<const u32*>(shader.code.data())
        };

        auto createResult = castVkResult(app.device.createShaderModule(shaderModuleCreateInfo));
        CHECK_KEEP(createResult);
        return createResult.take();
    }
}