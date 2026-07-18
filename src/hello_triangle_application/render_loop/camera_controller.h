#pragma once
#include "camera.h"
#include "../input/input_manager.h"

namespace HelloTriangleApplication::RenderLoop
{
    struct CameraControlConfig
    {
        f32 xzSpeed = 2.0f;
        f32 ySpeed = 1.0f;
        f32 sensitivity = 0.001f;
        bool followMouseStyle = false;
    };

    class CameraController
    {
    private:
        HelloTriangleApplication::Input::InputManager& inputs_;
        CameraControlConfig config_;

        void changePos(f32 delta);
        void changeRot(f32 delta);

    public:
        Camera handle;
        CameraControlConfig config;
        
        CameraController(Camera camera, HelloTriangleApplication::Input::InputManager& inputManager);

        void registerControls();
        void update(f32 delta);
    };
}