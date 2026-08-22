#pragma once
#include "common.h"
#include "vulkan_common.h"
#include "../resources/shader.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Utilities
{
    struct FatPipeline
    {
        vk::raii::Pipeline pipeline = nullptr;
        std::shared_ptr<vk::raii::PipelineLayout> layout = nullptr;

        vk::raii::Pipeline& operator*() { return this->pipeline; }
    };

    class PipelineBuilder
    {
    public:
        std::vector<vk::DynamicState> dynamicStates;                                // vk::PipelineDynamicStateCreateInfo
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
        std::vector<vk::VertexInputBindingDescription> vertBindingDescriptions;     // vk::PipelineVertexInputStateCreateInfo
        std::vector<vk::VertexInputAttributeDescription> vertAttributeDescriptions; // --
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
        std::vector<vk::Viewport> viewports;                                        // vk::PipelineViewportStateCreateInfo
        std::vector<vk::Rect2D> scissors;                                           // --
        vk::PipelineRasterizationStateCreateInfo rasterizationState;
        vk::PipelineMultisampleStateCreateInfo multisampleState;
        vk::PipelineDepthStencilStateCreateInfo depthStencilState;
        
        std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;   // vk::PipelineColorBlendStateCreateInfo
        Option<vk::LogicOp> colorBlendLogicOp;                                      // --
        vk::PipelineColorBlendStateCreateFlags colorBlendFlags;                     // --
        std::array<f32, 4> colorBlendConstants;                                     // --

        std::vector<vk::Format> colorAttachmentFormats;                             // vk::PipelineRenderingCreateInfo
        vk::Format depthAttachmentFormat;                                           // --
        vk::Format stencilAttachmentFormat;                                         // --

        std::shared_ptr<vk::raii::PipelineLayout> layout;

        /*
        Leaves only the following unpopulated:
            - shaderStages
            - vertBindingDescriptions
            - vertAttributeDescriptions
            - layout
        */
        void setDefaults(App& app);
        
        void addShaderStage(vk::ShaderStageFlagBits stage, vk::ShaderModule shader, Option<const char*> entryPoint = {});
        void addShaderStages(vk::ShaderModule shader, const std::vector<Resources::ShaderEntryPoint>& entryPoints);
        Result<FatPipeline, ErrorMessage> build(App& app) const;
    };

    class PipelineLayoutBuilder
    {
    public:
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;                  // vk::PipelineLayoutCreateInfo
        std::vector<vk::PushConstantRange> pushConstantRanges;                      // --

        Result<std::shared_ptr<vk::raii::PipelineLayout>, ErrorMessage> build(App& app) const;
    };
}