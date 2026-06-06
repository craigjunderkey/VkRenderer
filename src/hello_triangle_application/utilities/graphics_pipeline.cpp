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
    file.read(buffer.data(), (std::streamsize)(buffer.size()));

    // close up
    file.close();
    if (file.fail())
        return Err<std::string>(std::format("Failed to close file {}.", fileName));
    
    return std::move(buffer);
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createGraphicsPipeline(App& app)
    {
        std::vector<char> shaderCode;
        TRY_ASSIGN(shaderCode, readFile("shaders/shader.spv"));

        return Ok();
    }
}