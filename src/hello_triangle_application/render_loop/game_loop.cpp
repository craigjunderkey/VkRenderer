#include "../hello_triangle_application.h"
#include "game_loop.h"
#include "../utilities/push_constants.h"
#include "../geometry/mesh.h"

namespace HelloTriangleApplication::RenderLoop
{
    void GameLoop::initialize()
    {
        this->scene_ = {
            {
                .mesh = "utah_teapot",
                .material = "coulter",
                .transform { .s { 0.4f, 0.4f, 0.4f } }
            },
            {
                .mesh = "utah_teapot",
                .material = "laurent",
                .transform { .t { 0.0f, 0.5f, 1.25f }, .r { 0.0f, -0.4f, 0.0f }, .s { 0.15f, 0.15f, 0.15f} }
            }
        };
    }

    void GameLoop::startFrame()
    {
    }

    void GameLoop::endFrame()
    {
        this->frame_++;
    }
    
    void GameLoop::update(f32 delta)
    {
    }

    void GameLoop::draw(App& app, vk::raii::CommandBuffer& commandBuffer)
    {
        for (const RenderObject& obj : this->scene_)
        {
            GameLoop::submitDraw(app, commandBuffer, obj);
            // TODO: draw child
        }
    }

    void GameLoop::submitDraw(App& app, const vk::raii::CommandBuffer& commandBuffer, const RenderObject& obj)
    {
        // bind the geometry
        const Geometry::Mesh<u32>& mesh = app.resources.mesh(obj.mesh);
        commandBuffer.bindVertexBuffers(0, *mesh.vertexBuffer.buffer, {0});
        commandBuffer.bindIndexBuffer(*mesh.indexBuffer.buffer, 0, vk::IndexTypeValue<decltype(mesh.indices)::value_type>::value);

        // upload per-object data
        PushConstants pushConstants {
            .matId = app.resources.materialManager().getMaterialId(obj.material),
            .model = obj.transform.model()
        };
        commandBuffer.pushConstants(
            *app.pipelineLayout,
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            0, sizeof(PushConstants), &pushConstants
        );
        
        // draw the damn thing
        commandBuffer.drawIndexed(mesh.numIndices, 1, 0, 0, 0);
    }
}