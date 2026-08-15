#include "../hello_triangle_application.h"
#include "graphics_pipeline.h"
#include "../geometry/vertex.h"
#include "../geometry/vertex_info.h"
#include "push_constants.h"
#include "shader_module.h"
#include "../render_loop/light.h"
#include "../resources/shader.h"

using HelloTriangleApplication::App;
using HelloTriangleApplication::Geometry::Vertex;
using HelloTriangleApplication::Geometry::VertexInfo;
using HelloTriangleApplication::Resources::Shader;
using HelloTriangleApplication::Resources::ShaderStage;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createDefaultPipeline(App& app, PipelineBuilder& pipelineBuilder)
    {
        pipelineBuilder.setDefaults(app);
        
        // make programmable stages
        Shader& shader = app.resources.get<Shader>("scene");
        auto moduleResult = createShaderModule(app, shader);
        CHECK_KEEP(moduleResult);
        vk::raii::ShaderModule shaderModule = moduleResult.take();
        
        pipelineBuilder.addShaderStage(
            vk::ShaderStageFlagBits::eVertex,
            shaderModule,
            shader.getEntryPoint(ShaderStage::Vertex).functionName.data()
        );
        pipelineBuilder.addShaderStage(
            vk::ShaderStageFlagBits::eFragment, 
            shaderModule,
            shader.getEntryPoint(ShaderStage::Fragment).functionName.data()
        );

        // vertex input
        pipelineBuilder.vertBindingDescriptions = { VertexInfo::bindingDescription() };
        for (const auto& desc : VertexInfo::attributeDescriptions())
        {
            pipelineBuilder.vertAttributeDescriptions.emplace_back(desc);            
        }

        // pipeline layout
        PipelineLayoutBuilder layoutBuilder;
        layoutBuilder.descriptorSetLayouts = {
            *app.gpuMaterials.descriptorSetLayout(),
            *app.descriptorSetLayout
        };
        layoutBuilder.pushConstantRanges = {
            vk::PushConstantRange {
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .offset = 0,
                .size = sizeof(PushConstants)
            }
        };

        auto layoutBuildResult = layoutBuilder.build(app);
        CHECK_KEEP(layoutBuildResult);
        pipelineBuilder.layout = layoutBuildResult.take();

        // build pipeline
        auto pipelineBuildResult = pipelineBuilder.build(app);
        CHECK_KEEP(pipelineBuildResult);
        app.pipelines.add("scene", pipelineBuildResult.take());

        return Ok();
    }

    Result<Unit, std::string> createLightDebugPipeline(App& app, PipelineBuilder& pipelineBuilder)
    {
        // make programmable stages
        Shader& shader = app.resources.get<Shader>("light_debug");
        auto moduleResult = createShaderModule(app, shader);
        CHECK_KEEP(moduleResult);
        vk::raii::ShaderModule shaderModule = moduleResult.take();

        pipelineBuilder.shaderStages.clear();
        pipelineBuilder.addShaderStage(
            vk::ShaderStageFlagBits::eVertex,
            shaderModule,
            shader.getEntryPoint(ShaderStage::Vertex).functionName.data()
        );
        pipelineBuilder.addShaderStage(
            vk::ShaderStageFlagBits::eFragment,
            shaderModule,
            shader.getEntryPoint(ShaderStage::Fragment).functionName.data()
        );

        // pipeline layout
        PipelineLayoutBuilder layoutBuilder;
        layoutBuilder.descriptorSetLayouts = {};
        layoutBuilder.pushConstantRanges = {
            vk::PushConstantRange {
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .offset = 0,
                .size = sizeof(DebugLightPushConstants)
            }
        };
        
        auto layoutBuildResult = layoutBuilder.build(app);
        CHECK_KEEP(layoutBuildResult);
        pipelineBuilder.layout = layoutBuildResult.take();

        // build pipeline
        auto pipelineBuildResult = pipelineBuilder.build(app);
        CHECK_KEEP(pipelineBuildResult);
        app.pipelines.add("debug_light", pipelineBuildResult.take());
        
        return Ok();
    }

    Result<Unit, std::string> createGraphicsPipeline(App& app)
    {
        PipelineBuilder pipelineBuilder;
        CHECK(createDefaultPipeline(app, pipelineBuilder));
        CHECK(createLightDebugPipeline(app, pipelineBuilder));
        return Ok();
    }
}