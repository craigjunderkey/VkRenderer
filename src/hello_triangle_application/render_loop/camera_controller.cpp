#include "camera_controller.h"

using HelloTriangleApplication::Input::InputManager;

namespace HelloTriangleApplication::RenderLoop
{
    CameraController::CameraController(Camera camera, InputManager& inputManager)
        : handle(camera), inputs_(inputManager)
    {
    }

    void CameraController::registerControls()
    {
        inputs_.keyboard().registerAction("move_forward", { GLFW_KEY_W, GLFW_KEY_UP });
        inputs_.keyboard().registerAction("move_left", { GLFW_KEY_A, GLFW_KEY_LEFT });
        inputs_.keyboard().registerAction("move_back", { GLFW_KEY_S, GLFW_KEY_DOWN });
        inputs_.keyboard().registerAction("move_right", { GLFW_KEY_D, GLFW_KEY_RIGHT });
        inputs_.keyboard().registerAction("move_up", { GLFW_KEY_SPACE });
        inputs_.keyboard().registerAction("move_down", { GLFW_KEY_LEFT_SHIFT });
    }

    void CameraController::update(f32 delta)
    {
        if (this->inputs_.mouse().cursorMode() != CursorMode::Locked)
            return;
        
        changePos(delta);
        changeRot(delta);
    }

    void CameraController::changePos(f32 delta)
    {
        // get desired move
        vec2 wishDir = inputs_.keyboard().vector("move_left", "move_right", "move_forward", "move_back");
        f32 wishY = inputs_.keyboard().axis("move_down", "move_up");
        
        // calculate change in position
        vec3 deltaPos;
        if (this->config.followMouseStyle)
        {
            vec3 rawDeltaPos = vec3(
                wishDir.x * this->config.xzSpeed,
                wishY * this->config.ySpeed,
                wishDir.y * this->config.xzSpeed
            ) * delta;
            deltaPos = this->handle.basis() * vec4(rawDeltaPos, 0.0f);
        }
        else
        {
            vec3 xzDir = glm::rotate(mat4(1.0f), -this->handle.transform().yRot, vec3(0.0f, 1.0f, 0.0f))
                * vec4(wishDir.x, 0.0f, wishDir.y, 0.0f);
            
            deltaPos = vec3(
                xzDir.x * this->config.xzSpeed,
                wishY * this->config.ySpeed,
                xzDir.z * this->config.xzSpeed
            ) * delta;
        }
        
        // apply change
        CameraTransform xform = this->handle.transform();
        xform.pos += deltaPos;
        this->handle.setTransform(xform);
    }

    void CameraController::changeRot(f32 delta)
    {
        // get desired move
        vec2 mouseDelta = this->inputs_.mouse().delta();

        // calculate change in rotation
        vec2 deltaRot = vec2(-mouseDelta.y, mouseDelta.x) * this->config.sensitivity;

        // apply change
        CameraTransform xform = this->handle.transform();
        xform.xRot += deltaRot.x;
        xform.yRot += deltaRot.y;
        this->handle.setTransform(xform);
    }
}