#pragma once
#include "keyboard_controller.h"
#include "../window.h"

namespace HelloTriangleApplication::Input
{
    class InputManager
    {
    private:
        KeyboardController keyboard_;

    public:
        InputManager(Window& window)
            : keyboard_(KeyboardController(window))
        {
        }
        
        inline KeyboardController& keyboard() { return this->keyboard_; }

        void startFrame()
        {
            this->keyboard_.startFrame();
        }

        void endFrame()
        {
            this->keyboard_.endFrame();
        }
    };
}