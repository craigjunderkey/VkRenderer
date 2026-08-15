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
    Result<Unit, std::string> createScenePipeline(App& app, PipelineBuilder& pipelineBuilder)
    {
        pipelineBuilder.setDefaults(app);
        
        // make programmable stages
        Shader& shader = app.resources.get<Shader>("scene");
        auto moduleResult = createShaderModule(app, shader);
        CHECK_KEEP(moduleResult);
        vk::raii::ShaderModule shaderModule = moduleResult.take();
        
        pipelineBuilder.addShaderStages(shaderModule, shader.entryPoints);

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

    Result<Unit, std::string> createSkyboxPipeline(App& app, PipelineBuilder& pipelineBuilder)
    {
        // make programmable stages
        Shader& shader = app.resources.get<Shader>("skybox");
        auto moduleResult = createShaderModule(app, shader);
        CHECK_KEEP(moduleResult);
        vk::raii::ShaderModule shaderModule = moduleResult.take();

        pipelineBuilder.shaderStages.clear();
        pipelineBuilder.addShaderStages(shaderModule, shader.entryPoints);

        // set depth test to <=
        pipelineBuilder.depthStencilState.depthCompareOp = vk::CompareOp::eLessOrEqual;

        // build pipeline
        auto pipelineBuildResult = pipelineBuilder.build(app);
        CHECK_KEEP(pipelineBuildResult);
        app.pipelines.add("skybox", pipelineBuildResult.take());

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
        pipelineBuilder.addShaderStages(shaderModule, shader.entryPoints);

        // set depth test to <
        pipelineBuilder.depthStencilState.depthCompareOp = vk::CompareOp::eLess;

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
        CHECK(createScenePipeline(app, pipelineBuilder));
        CHECK(createSkyboxPipeline(app, pipelineBuilder));
        CHECK(createLightDebugPipeline(app, pipelineBuilder));
        return Ok();
    }
}