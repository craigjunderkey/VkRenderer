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
        f32 xzSpeedIncrement = 1.0f;
        f32 ySpeedIncrement = 0.5f;
        bool followMouseStyle = false;
        bool logCameraChanges = CONFIG.logByDefault;
    };

    class CameraController
    {
    private:
        HelloTriangleApplication::Input::InputManager& inputs_;

        void changeSpeed();
        void changeMode();
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