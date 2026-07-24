#include "../hello_triangle_application.h"
#include "shader_module.h"
#include <fstream>

namespace HelloTriangleApplication::Utilities
{
    Result<std::vector<char>, std::string> readCode(const char* fileName)
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

    Result<vk::raii::ShaderModule, std::string> loadShaderModule(App& app, const char* fileName)
    {
        auto readCodeResult = readCode(fileName);
        CHECK_KEEP(readCodeResult);
        std::vector<char> code = readCodeResult.take();
        return createShaderModule(app, code);
    }
}