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
        vec3 rawDeltaPos = vec3(
            wishDir.x * this->xzSpeed_,
            wishY * this->ySpeed_,
            wishDir.y * this->xzSpeed_
        ) * delta;
        vec3 deltaPos = this->handle.basis() * vec4(rawDeltaPos, 0.0f);
        
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
        vec2 deltaRot = vec2(-mouseDelta.y, mouseDelta.x) * this->sensitivity_;

        // apply change
        CameraTransform xform = this->handle.transform();
        xform.xRot += deltaRot.x;
        xform.yRot += deltaRot.y;
        this->handle.setTransform(xform);
    }
}