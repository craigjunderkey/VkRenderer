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
        vec2 wishDir = inputs_.keyboard().vector("move_left", "move_right", "move_forward", "move_back");
        f32 wishY = inputs_.keyboard().axis("move_down", "move_up");
        vec3 deltaPos = vec3(
            wishDir.x * this->xzSpeed_,
            wishY * this->ySpeed_,
            wishDir.y * this->xzSpeed_
        ) * delta;
        
        CameraTransform xform = this->handle.transform();
        xform.pos += deltaPos;
        this->handle.setTransform(xform);
    }
}