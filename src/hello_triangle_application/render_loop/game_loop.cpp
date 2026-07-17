#include "../hello_triangle_application.h"
#include "game_loop.h"
#include "../utilities/push_constants.h"
#include "../geometry/mesh.h"
#include "animations/spin.h"
#include "animations/wobble.h"

namespace HelloTriangleApplication::RenderLoop
{
    void GameLoop::initialize()
    {
        this->scene_.emplace_back(
            RenderObject(
                "utah_teapot",
                "coulter",
                Transform { .s { 0.4f, 0.4f, 0.4f } },
                std::make_unique<Animations::Wobble>(Animations::Wobble(1.25f, 0.35f, 0.0f))
            )
        );
        this->scene_.emplace_back(
            RenderObject(
                "utah_teapot",
                "laurent",
                Transform { .t { 0.0f, 0.5f, 1.25f }, .r { 0.0f, -0.4f, 0.0f }, .s { 0.15f, 0.15f, 0.15f} },
                std::make_unique<Animations::Spin>(Animations::Spin(-0.785f))
            )
        );
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
        for (RenderObject& obj : this->scene_)
        {
            if (!obj.animation) continue;

            AnimationResult result = obj.animation->animate(obj.transform.trs(), (f32)glfwGetTime(), delta);
            obj.transform.setFromTrs(std::move(result.out));
            if (result.newAnim)
                obj.animation = std::move(result.newAnim).value();
        }
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
            .model = obj.transform.global()
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