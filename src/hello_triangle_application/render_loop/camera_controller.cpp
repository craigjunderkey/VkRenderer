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
        inputs_.keyboard().registerAction("cam_move_forward", { GLFW_KEY_W, GLFW_KEY_UP });
        inputs_.keyboard().registerAction("cam_move_left", { GLFW_KEY_A, GLFW_KEY_LEFT });
        inputs_.keyboard().registerAction("cam_move_back", { GLFW_KEY_S, GLFW_KEY_DOWN });
        inputs_.keyboard().registerAction("cam_move_right", { GLFW_KEY_D, GLFW_KEY_RIGHT });
        inputs_.keyboard().registerAction("cam_move_up", { GLFW_KEY_SPACE });
        inputs_.keyboard().registerAction("cam_move_down", { GLFW_KEY_LEFT_SHIFT });
        inputs_.keyboard().registerAction("cam_change_mode", { GLFW_KEY_C });
    }

    void CameraController::update(f32 delta)
    {
        if (this->inputs_.mouse().cursorMode() != CursorMode::Locked)
            return;
        
        changeMode();
        changeSpeed();
        changePos(delta);
        changeRot(delta);
    }

    void CameraController::changePos(f32 delta)
    {
        // get desired move
        vec2 wishDir = inputs_.keyboard().vector("cam_move_left", "cam_move_right", "cam_move_forward", "cam_move_back");
        f32 wishY = inputs_.keyboard().axis("cam_move_down", "cam_move_up");
        
        // calculate change in position
        vec3 deltaPos;
        if (this->config.followMouseStyle)
        {
            deltaPos = vec3(
                wishDir.x * this->config.xzSpeed,
                0.0f,
                wishDir.y * this->config.xzSpeed
            ) * delta;
            deltaPos = this->handle.basis() * vec4(deltaPos, 0.0f);
            deltaPos.y += wishY * this->config.ySpeed * delta;
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

    void CameraController::changeSpeed()
    {
        // get mouse wheel delta
        f32 delta = this->inputs_.mouse().scrollDelta().y;
        if (delta == 0.0f)
            return;
        
        // change xz speed
        vec2 wishXz = inputs_.keyboard().vector("cam_move_left", "cam_move_right", "cam_move_forward", "cam_move_back");
        if (VectorOps::lengthSquared(wishXz) > VectorOps::EPSILON)
        {
            f32 newSpeed = this->config.xzSpeed + delta * this->config.xzSpeedIncrement;
            if (newSpeed < VectorOps::EPSILON)
                return;
            this->config.xzSpeed = newSpeed;
            
            if (this->config.logCameraChanges)
                std::cout << "Camera xz-speed changed to " << this->config.xzSpeed << std::endl;
            
            return;
        }
        
        // change y speed
        f32 wishY = inputs_.keyboard().axis("cam_move_down", "cam_move_up");
        if (glm::abs(wishY) > VectorOps::EPSILON)
        {
            f32 newSpeed = this->config.ySpeed + delta * this->config.ySpeedIncrement;
            if (newSpeed < VectorOps::EPSILON)
                return;
            this->config.ySpeed = newSpeed;

            if (this->config.logCameraChanges)
                std::cout << "Camera y-speed changed to " << this->config.ySpeed << std::endl;
            
            return;
        }
    }

    void CameraController::changeMode()
    {
        if (this->inputs_.keyboard().pressed("cam_change_mode"))
        {
            this->config.followMouseStyle = !this->config.followMouseStyle;
            
            if (this->config.logCameraChanges)
            {
                if (this->config.followMouseStyle)
                {
                    std::cout << "Camera now following mouse\n";
                }
                else
                {
                    std::cout << "Camera no longer following mouse\n";
                }
            }
        }
    }
}