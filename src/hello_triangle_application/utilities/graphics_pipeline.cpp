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

Result<std::array<vk::PipelineShaderStageCreateInfo, 2>, std::string> makeProgrammableStages(App& app)
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

    return std::array<vk::PipelineShaderStageCreateInfo, 2> {
        std::move(vertShaderStageInfo),
        std::move(fragShaderStageInfo)
    };
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createGraphicsPipeline(App& app)
    {
        // dynamic state
        std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo {
            .dynamicStateCount = static_cast<u32>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };

        // store programmable stages
        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;
        TRY_ASSIGN(shaderStages, makeProgrammableStages(app));

        // vertex input
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

        // input assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo {
            .topology = vk::PrimitiveTopology::eTriangleList
        };

        // viewport and scissors
        vk::Viewport viewport {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<f32>(app.swapChainExtent.width),
            .height = static_cast<f32>(app.swapChainExtent.height),
            .minDepth = 0.0f, .maxDepth = 1.0f
        };
        vk::Rect2D scissor {
            .offset = vk::Offset2D {0, 0},
            .extent = app.swapChainExtent
        };
        vk::PipelineViewportStateCreateInfo viewportStateInfo {
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor
        };

        // rasterization
        vk::PipelineRasterizationStateCreateInfo rasterizationStateInfo {
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eBack,
            .frontFace = vk::FrontFace::eClockwise,
            .depthBiasEnable = false,
            .lineWidth = 1.0f
        };

        // multisampling
        vk::PipelineMultisampleStateCreateInfo multisampleStateInfo {
            .rasterizationSamples = vk::SampleCountFlagBits::e1,
            .sampleShadingEnable = false
        };

        // color blending
        vk::PipelineColorBlendAttachmentState colorBlendAttachment {
            .blendEnable = true,
            .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
            .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
            .colorBlendOp = vk::BlendOp::eAdd,
            .srcAlphaBlendFactor = vk::BlendFactor::eOne,
            .dstAlphaBlendFactor = vk::BlendFactor::eZero,
            .alphaBlendOp = vk::BlendOp::eAdd,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        };
        vk::PipelineColorBlendStateCreateInfo colorBlending {
            .logicOpEnable = false,
            .logicOp = vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment
        };

        // pipeline layout
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
            .setLayoutCount = 0,
            .pushConstantRangeCount = 0
        };
        TRY_ASSIGN(
            app.pipelineLayout,
            castVkResult(app.device.createPipelineLayout(pipelineLayoutInfo))
        );

        return Ok();
    }
}