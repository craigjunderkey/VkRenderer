#pragma once
#include "window.h"
#include <unordered_map>

namespace HelloTriangleApplication
{
    class KeyboardController
    {
    private:
        bool space_;
        bool prevSpace_;

    public:
        void update(Window& window)
        {
            prevSpace_ = space_;
            space_ = glfwGetKey(*window, GLFW_KEY_SPACE) == GLFW_PRESS; 
        }

        bool pressedSpace() { return space_ && !prevSpace_; }
        bool releasedSpace() { return !space_ && prevSpace_; }
    };
}