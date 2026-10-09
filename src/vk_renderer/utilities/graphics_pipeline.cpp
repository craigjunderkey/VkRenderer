#include "../vk_renderer.h"
#include "graphics_pipeline.h"
#include "../geometry/vertex.h"
#include "../geometry/vertex_info.h"
#include "push_constants.h"
#include "shader_module.h"
#include "../render_loop/light.h"
#include "../resources/shader.h"
#include "../particles/particle_vertex_info.h"

using VkRenderer::App;
using VkRenderer::Geometry::Vertex;
using VkRenderer::Geometry::VertexInfo;
using VkRenderer::Resources::Shader;
using VkRenderer::Resources::ShaderStage;

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createScenePipeline(App& app, PipelineBuilder& pipelineBuilder)
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
            *app.resources.bindless().getDescriptorSetLayout(),
            *app.descriptorSetLayout
        };
        layoutBuilder.pushConstantRanges = {
            vk::PushConstantRange {
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .offset = 0,
                .size = sizeof(ScenePushConstants)
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

    Result<Unit, ErrorMessage> createSkyboxPipeline(App& app, PipelineBuilder& pipelineBuilder)
    {
        // make programmable stages
        Shader& shader = app.resources.get<Shader>("skybox");
        auto moduleResult = createShaderModule(app, shader);
        CHECK_KEEP(moduleResult);
        vk::raii::ShaderModule shaderModule = moduleResult.take();

        pipelineBuilder.shaderStages.clear();
        pipelineBuilder.addShaderStages(shaderModule, shader.entryPoints);

        // truncate vertex attributes (not using normals or texcoords)
        pipelineBuilder.vertAttributeDescriptions.clear();
        pipelineBuilder.vertAttributeDescriptions.emplace_back(VertexInfo::attributeDescriptions()[0]);

        // set depth test to <=
        pipelineBuilder.depthStencilState.depthCompareOp = vk::CompareOp::eLessOrEqual;

        // build pipeline
        auto pipelineBuildResult = pipelineBuilder.build(app);
        CHECK_KEEP(pipelineBuildResult);
        app.pipelines.add("skybox", pipelineBuildResult.take());

        return Ok();
    }

    Result<Unit, ErrorMessage> createLightDebugPipeline(App& app, PipelineBuilder& pipelineBuilder)
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

    Result<Unit, ErrorMessage> createParticleRenderPipeline(App& app, PipelineBuilder& pipelineBuilder)
    {
        // make programmable stages
        Shader& shader = app.resources.get<Shader>("particle");
        auto moduleResult = createShaderModule(app, shader);
        CHECK_KEEP(moduleResult);
        vk::raii::ShaderModule shaderModule = moduleResult.take();

        pipelineBuilder.shaderStages.clear();
        pipelineBuilder.addShaderStage(
            vk::ShaderStageFlagBits::eVertex,
            shaderModule,
            shader.getEntryPoint(Resources::ShaderStage::Vertex).functionName.c_str()
        );
        pipelineBuilder.addShaderStage(
            vk::ShaderStageFlagBits::eFragment,
            shaderModule,
            shader.getEntryPoint(Resources::ShaderStage::Fragment).functionName.c_str()
        );

        // pipeline layout
        PipelineLayoutBuilder layoutBuilder;
        layoutBuilder.descriptorSetLayouts = {
            *app.resources.bindless().getDescriptorSetLayout(),
            *app.descriptorSetLayout
        };
        layoutBuilder.pushConstantRanges = {};
        auto layoutResult = layoutBuilder.build(app);
        CHECK_KEEP(layoutResult);
        pipelineBuilder.layout = layoutResult.take();

        // vertex input
        pipelineBuilder.vertBindingDescriptions = { Particles::getParticleVertexBinding() };
        pipelineBuilder.vertAttributeDescriptions.clear();
        for (const auto& desc : Particles::getParticleVertexAttributes())
        {
            pipelineBuilder.vertAttributeDescriptions.emplace_back(desc);            
        }

        // topology
        pipelineBuilder.inputAssembly.topology = vk::PrimitiveTopology::ePointList;

        // build pipeline
        auto pipelineBuildResult = pipelineBuilder.build(app);
        CHECK_KEEP(pipelineBuildResult);
        app.pipelines.add("particle_render", pipelineBuildResult.take());
        
        return Ok();
    }

    Result<Unit, ErrorMessage> createGraphicsPipeline(App& app)
    {
        PipelineBuilder pipelineBuilder;
        CHECK(createScenePipeline(app, pipelineBuilder));
        CHECK(createSkyboxPipeline(app, pipelineBuilder));
        CHECK(createLightDebugPipeline(app, pipelineBuilder));
        CHECK(createParticleRenderPipeline(app, pipelineBuilder));
        return Ok();
    }
}