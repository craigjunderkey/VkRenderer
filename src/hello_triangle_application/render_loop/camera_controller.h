#pragma once
#include "camera.h"
#include "../input/input_manager.h"

namespace HelloTriangleApplication::RenderLoop
{
    class CameraController
    {
    private:
        HelloTriangleApplication::Input::InputManager& inputs_;
        f32 xzSpeed_ = 1.0f;
        f32 ySpeed_ = 0.5f;

        void changePos(f32 delta);
        void changeRot(f32 delta);

    public:
        Camera handle;
        
        CameraController(Camera camera, HelloTriangleApplication::Input::InputManager& inputManager);

        void registerControls();
        void update(f32 delta);
    };
}