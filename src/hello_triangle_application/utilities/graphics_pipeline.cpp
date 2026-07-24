#include "../hello_triangle_application.h"
#include "graphics_pipeline.h"
#include "../geometry/vertex.h"
#include "push_constants.h"
#include "shader_module.h"

using HelloTriangleApplication::App;
using HelloTriangleApplication::Geometry::Vertex;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createGraphicsPipeline(App& app)
    {
        PipelineBuilder pipelineBuilder;
        pipelineBuilder.setDefaults(app);
        
        // make programmable stages
        auto shaderLoadResult = loadShaderModule(app, SHADER_PATH);
        CHECK_KEEP(shaderLoadResult);
        vk::raii::ShaderModule shaderModule = shaderLoadResult.take();
        
        pipelineBuilder.addShaderStage(vk::ShaderStageFlagBits::eVertex, shaderModule);
        pipelineBuilder.addShaderStage(vk::ShaderStageFlagBits::eFragment, shaderModule);

        // vertex input
        pipelineBuilder.vertBindingDescriptions = { Vertex::bindingDescription() };
        for (const auto& desc : Vertex::attributeDescriptions())
        {
            pipelineBuilder.vertAttributeDescriptions.emplace_back(desc);            
        }

        // pipeline layout
        PipelineLayoutBuilder layoutBuilder;
        layoutBuilder.descriptorSetLayouts = {
            *app.resources.materialManager().descriptorSetLayout(),
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
        app.pipeline = pipelineBuildResult.take();

        return Ok();
    }
}