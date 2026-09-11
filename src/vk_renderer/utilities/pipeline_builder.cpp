#include "../vk_renderer.h"
#include "pipeline_builder.h"
#include "shader_module.h"

using VkRenderer::Resources::ShaderStage;

namespace VkRenderer::Utilities
{
    void PipelineBuilder::setDefaults(App& app)
    {
        this->dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        this->inputAssembly = { .topology = vk::PrimitiveTopology::eTriangleList };
        this->viewports = {
            vk::Viewport {
                .x = 0.0f, .y = 0.0f,
                .width = static_cast<f32>(app.swapchain.getExtent().width),
                .height = static_cast<f32>(app.swapchain.getExtent().height),
                .minDepth = 0.0f, .maxDepth = 1.0f
            }
        };
        this->scissors = {
            vk::Rect2D {
                .offset = vk::Offset2D {0, 0},
                .extent = app.swapchain.getExtent()
            }
        };
        this->rasterizationState = {
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eBack,
            .frontFace = vk::FrontFace::eCounterClockwise,
            .depthBiasEnable = false,
            .lineWidth = 1.0f
        };
        this->multisampleState = {
            .rasterizationSamples = app.msaaSamples,
            .sampleShadingEnable = false
        };
        this->depthStencilState = {
            .depthTestEnable = vk::True,
            .depthWriteEnable = vk::True,
            .depthCompareOp = vk::CompareOp::eLess,
            .depthBoundsTestEnable = vk::False,
            .stencilTestEnable = vk::False
        };
        this->colorBlendAttachments = {
            vk::PipelineColorBlendAttachmentState {
                .blendEnable = true,
                .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
                .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
                .colorBlendOp = vk::BlendOp::eAdd,
                .srcAlphaBlendFactor = vk::BlendFactor::eOne,
                .dstAlphaBlendFactor = vk::BlendFactor::eZero,
                .alphaBlendOp = vk::BlendOp::eAdd,
                .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                    | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
            }
        };
        this->colorBlendLogicOp = {};
        this->colorAttachmentFormats = { app.swapchain.getFormat().format };
        this->depthAttachmentFormat = app.depthTexture.info.format;
        this->stencilAttachmentFormat = vk::Format::eUndefined;
    }

    void PipelineBuilder::addShaderStage(vk::ShaderStageFlagBits stage, vk::ShaderModule shader, Option<const char*> entryPoint)
    {
        // get entry point name
        const char* name;
        if (entryPoint)
        {
            name = entryPoint.value();
        }
        else
        {
            switch (stage)
            {
                case vk::ShaderStageFlagBits::eVertex:
                    name = "vertMain";
                    break;
                case vk::ShaderStageFlagBits::eFragment:
                    name = "fragMain";
                    break;
                default:
                    err({ .channel = "vulkan" }, "Shader missing entry point name");
                    panic();
            }
        }

        // add shader stage
        this->shaderStages.emplace_back(
            vk::PipelineShaderStageCreateInfo {
                .stage = stage,
                .module = shader,
                .pName = name
            }
        );
    }

    void PipelineBuilder::addShaderStages(vk::ShaderModule shader, const std::vector<Resources::ShaderEntryPoint>& entryPoints)
    {
        for (const Resources::ShaderEntryPoint& entryPoint : entryPoints)
        {
            // get stage bit
            vk::ShaderStageFlagBits stage;
            switch (entryPoint.stage)
            {
                case ShaderStage::Vertex:
                    stage = vk::ShaderStageFlagBits::eVertex;
                    break;
                case ShaderStage::Fragment:
                    stage = vk::ShaderStageFlagBits::eFragment;
                    break;
                case ShaderStage::Compute:
                    stage = vk::ShaderStageFlagBits::eCompute;
                    break;
                case ShaderStage::Geometry:
                    stage = vk::ShaderStageFlagBits::eGeometry;
                    break;
                case ShaderStage::Hull:
                    stage = vk::ShaderStageFlagBits::eTessellationControl;
                    break;
                case ShaderStage::Domain:
                    stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
                    break;
                case ShaderStage::RayGeneration:
                    stage = vk::ShaderStageFlagBits::eRaygenKHR;
                    break;
            }

            // add shader stage
            this->shaderStages.emplace_back(
                vk::PipelineShaderStageCreateInfo {
                    .stage = stage,
                    .module = shader,
                    .pName = entryPoint.functionName.data()
                }
            );
        }
    }


    Result<FatPipeline, ErrorMessage> PipelineBuilder::build(App& app) const
    {
        // dynamic state
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo {
            .dynamicStateCount = static_cast<u32>(this->dynamicStates.size()),
            .pDynamicStates = this->dynamicStates.data()
        };

        // vertex input state
        vk::PipelineVertexInputStateCreateInfo vertInputInfo {
            .vertexBindingDescriptionCount = static_cast<u32>(this->vertBindingDescriptions.size()),
            .pVertexBindingDescriptions = this->vertBindingDescriptions.data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(this->vertAttributeDescriptions.size()),
            .pVertexAttributeDescriptions = this->vertAttributeDescriptions.data()
        };

        // viewport state
        vk::PipelineViewportStateCreateInfo viewportStateInfo {
            .viewportCount = static_cast<u32>(this->viewports.size()),
            .pViewports = this->viewports.data(),
            .scissorCount = static_cast<u32>(this->scissors.size()),
            .pScissors = this->scissors.data()
        };

        // color blend state
        vk::LogicOp logicOp = vk::LogicOp::eCopy;
        if (this->colorBlendLogicOp)
            logicOp = this->colorBlendLogicOp.value();
        vk::PipelineColorBlendStateCreateInfo colorBlendState {
            .logicOpEnable = this->colorBlendLogicOp.has_value(),
            .logicOp = logicOp,
            .attachmentCount = static_cast<u32>(this->colorBlendAttachments.size()),
            .pAttachments = this->colorBlendAttachments.data()
        };

        // render info
        vk::PipelineRenderingCreateInfo renderInfo {
            .colorAttachmentCount = static_cast<u32>(this->colorAttachmentFormats.size()),
            .pColorAttachmentFormats = this->colorAttachmentFormats.data(),
            .depthAttachmentFormat = this->depthAttachmentFormat,
            .stencilAttachmentFormat = this->stencilAttachmentFormat
        };

        // make the pipeline
        vk::GraphicsPipelineCreateInfo pipelineInfo {
            .pNext = &renderInfo,
            .stageCount = static_cast<u32>(this->shaderStages.size()),
            .pStages = this->shaderStages.data(),
            .pVertexInputState = &vertInputInfo,
            .pInputAssemblyState = &this->inputAssembly,
            .pViewportState = &viewportStateInfo,
            .pRasterizationState = &this->rasterizationState,
            .pMultisampleState = &this->multisampleState,
            .pDepthStencilState = &this->depthStencilState,
            .pColorBlendState = &colorBlendState,
            .pDynamicState = &dynamicStateInfo,
            .layout = **this->layout,
            .renderPass = nullptr
        };

        auto pipelineResult = castVkResult(app.device.createGraphicsPipeline(nullptr, pipelineInfo));
        CHECK_KEEP(pipelineResult);

        return FatPipeline {
            .pipeline = pipelineResult.take(),
            .layout = this->layout
        };
    }

    Result<std::shared_ptr<vk::raii::PipelineLayout>, ErrorMessage> PipelineLayoutBuilder::build(App& app) const
    {
        vk::PipelineLayoutCreateInfo createInfo {
            .setLayoutCount = static_cast<u32>(this->descriptorSetLayouts.size()),
            .pSetLayouts = this->descriptorSetLayouts.data(),
            .pushConstantRangeCount = static_cast<u32>(this->pushConstantRanges.size()),
            .pPushConstantRanges = this->pushConstantRanges.data()
        };

        VkPipelineLayout layout;
        
        auto layoutResult = castVkResult(vkCreatePipelineLayout(*app.device, createInfo, nullptr, &layout));
        CHECK(layoutResult);

        return std::make_shared<vk::raii::PipelineLayout>(vk::raii::PipelineLayout(app.device, layout));
    }
}