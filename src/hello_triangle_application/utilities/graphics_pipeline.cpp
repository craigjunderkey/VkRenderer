#include "../hello_triangle_application.h"
#include "graphics_pipeline.h"
#include <fstream>

using HelloTriangleApplication::App;

Result<std::vector<char>, std::string> readFile(const std::string& fileName)
{
    // open the file at its end
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        return Err<std::string>(std::format("Failed to open file {}.", fileName));
    
    // since we start at the end of the file, this means we allocate the file length
    std::vector<char> buffer(file.tellg());
    
    // return to beginning and read in the bytes
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    // close up
    file.close();
    if (file.fail())
        return Err<std::string>(std::format("Failed to close file {}.", fileName));
    
    return std::move(buffer);
}

Result<vk::raii::ShaderModule, std::string> createShaderModule(App& app, const std::vector<char>& code)
{
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo {
        .codeSize = code.size() * sizeof(char),
        .pCode = reinterpret_cast<const u32*>(code.data())
    };

    vk::raii::ShaderModule shaderModule = nullptr;
    TRY_ASSIGN(shaderModule, castVkResult(app.device.createShaderModule(shaderModuleCreateInfo)));

    return shaderModule;
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createGraphicsPipeline(App& app)
    {
        std::vector<char> shaderCode;
        TRY_ASSIGN(shaderCode, readFile(SHADER_PATH));

        vk::raii::ShaderModule shaderModule = nullptr;
        TRY_ASSIGN(shaderModule, createShaderModule(app, std::move(shaderCode)));

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo {
            .stage = vk::ShaderStageFlagBits::eVertex,
            .module = shaderModule,
            .pName = "vertMain"
        };

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo {
            .stage = vk::ShaderStageFlagBits::eFragment,
            .module = shaderModule,
            .pName = "fragMain"
        };

        vk::PipelineShaderStageCreateInfo shaderStages[] {
            std::move(vertShaderStageInfo),
            std::move(fragShaderStageInfo)
        };

        return Ok();
    }
}