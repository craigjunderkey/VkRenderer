#pragma once
#include "keyboard_controller.h"
#include "mouse_controller.h"
#include "../window.h"

namespace VkRenderer::Input
{
    class InputManager
    {
    private:
        KeyboardController keyboard_;
        MouseController mouse_;

    public:
        InputManager(Window& window)
            : keyboard_(KeyboardController(window)), mouse_(MouseController(window))
        {
        }
        
        inline KeyboardController& keyboard() { return this->keyboard_; }
        inline MouseController& mouse() { return this->mouse_; }

        void startFrame()
        {
            this->keyboard_.startFrame();
            this->mouse_.startFrame();
        }

        void endFrame()
        {
            this->mouse_.endFrame();
            this->keyboard_.endFrame();
        }
    };
}