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
        // input setup
        this->app_.inputs.keyboard().registerAction("quit", { GLFW_KEY_Q });
        this->app_.inputs.keyboard().registerAction("free_mouse", { GLFW_KEY_ESCAPE });

        // camera setup
        this->app_.camera.handle.setTransform(
            CameraTransform {
                .pos = vec3(0.0f, 2.0f, 3.0f),
                .xRot = -glm::pi<f32>() / 7,
                .yRot = 0.0f
            }
        );

        // build scene
        this->scene_.emplace_back(
            RenderObject(
                "sphere",
                "coulter",
                Transform { .s { 0.4f, 0.4f, 0.4f } },
                std::make_unique<Animations::Wobble>(Animations::Wobble(0.785, 0.5f, 0.0f))
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
        this->scene_[1].addChild(
            RenderObject(
                "utah_teapot",
                "fantl",
                Transform { .t { 0.0f, 3.5f, 0.0f }, .s { 0.5f, 0.5f, 0.5f } },
                std::make_unique<Animations::Spin>(Animations::Spin(1.65f))
            )
        );

        // add lights
        PointLight& light = this->app_.environment.pointLights[this->app_.environment.numPointLights];
        light = PointLight { .position = vec3(0.0f, 1.0f, 3.0f) };
        this->app_.environment.numPointLights++;
    }

    void GameLoop::startFrame()
    {
    }

    void GameLoop::endFrame()
    {
        this->frame_++;
    }

    void animateObject(RenderObject& obj, f32 delta)
    {
        if (obj.animation)
        {
            AnimationResult result = obj.animation->animate(obj.transform.trs(), (f32)glfwGetTime(), delta);
            obj.transform.setFromTrs(std::move(result.out));
            if (result.newAnim)
                obj.animation = std::move(result.newAnim).value();
        }

        for (RenderObject& child : obj.children)
        {
            animateObject(child, delta);
        }
    }
    
    void GameLoop::update(f32 delta)
    {
        // handle inputs
        if (this->app_.inputs.keyboard().released("quit"))
            this->app_.window.status.markedForClosure = true;
        if (this->app_.inputs.keyboard().released("free_mouse"))
            this->app_.inputs.mouse().setCursorMode(CursorMode::Default);
        if (this->app_.inputs.mouse().pressed(MouseButton::Left))
            this->app_.inputs.mouse().setCursorMode(CursorMode::Locked);
        
        // animate each object in the scene
        for (RenderObject& obj : this->scene_)
        {
            animateObject(obj, delta);
        }
    }

    void GameLoop::draw(vk::raii::CommandBuffer& commandBuffer)
    {
        for (const RenderObject& obj : this->scene_)
        {
            GameLoop::submitDraw(this->app_, commandBuffer, obj);
            
            for (const RenderObject& child : obj.children)
            {
                GameLoop::submitDraw(this->app_, commandBuffer, child);
            }
        }
    }

    void GameLoop::submitDraw(App& app, const vk::raii::CommandBuffer& commandBuffer, const RenderObject& obj)
    {
        // bind the geometry
        const Geometry::Mesh<u32>& mesh = app.resources.mesh(obj.mesh);
        commandBuffer.bindVertexBuffers(0, *mesh.vertexBuffer.buffer, {0});
        commandBuffer.bindIndexBuffer(*mesh.indexBuffer.buffer, 0, vk::IndexTypeValue<decltype(mesh.indices)::value_type>::value);

        // upload per-object data
        mat4 model = obj.transform.global();
        PushConstants pushConstants {
            .matId = app.resources.materialManager().getMaterialId(obj.material),
            .model = model,
            .normal = glm::transpose(glm::inverse(app.view * model))
        };
        commandBuffer.pushConstants(
            *app.pipeline.layout,
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            0, sizeof(PushConstants), &pushConstants
        );
        
        // draw the damn thing
        commandBuffer.drawIndexed(mesh.numIndices, 1, 0, 0, 0);
    }
}