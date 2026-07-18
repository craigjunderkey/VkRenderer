#include "mouse_controller.h"

namespace HelloTriangleApplication::Input
{
    vec2 MouseController::scrollDelta_ = vec2(0.0f, 0.0f);

    MouseController::MouseController(Window& window) : window_(window)
    {
        this->mouseButtons_ = { false };
        this->prevMouseButtons_ = { false };
    }

    void MouseController::startFrame()
    {
        // register scrollback if we haven't already
        if (!this->hasRegisteredCallback_)
        {
            glfwSetScrollCallback(*this->window_, scrollCallback);
            this->hasRegisteredCallback_ = true;
        }
        
        // update mouse buttons
        this->mouseButtons_[MouseButton::Left] = glfwGetMouseButton(*this->window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        this->mouseButtons_[MouseButton::Right] = glfwGetMouseButton(*this->window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        this->mouseButtons_[MouseButton::Middle] = glfwGetMouseButton(*this->window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
    }

    void MouseController::endFrame()
    {
        this->prevMouseButtons_.swap(this->mouseButtons_);
        this->prevPos_ = this->pos();
        MouseController::scrollDelta_ = vec2(0.0f, 0.0f);
    }

    bool MouseController::pressing(MouseButton button) const
    {
        return this->mouseButtons_[button];
    }

    bool MouseController::pressed(MouseButton button) const
    {
        return this->mouseButtons_[button] && !this->prevMouseButtons_[button];
    }

    bool MouseController::released(MouseButton button) const
    {
        return !this->mouseButtons_[button] && this->prevMouseButtons_[button];
    }

    vec2 MouseController::pos() const
    {
        f64 x, y;
        glfwGetCursorPos(*this->window_, &x, &y);
        return vec2(x, y);
    }

    vec2 MouseController::delta() const
    {
        return this->pos() - this->prevPos_;
    }

    bool MouseController::insideWindow() const
    {
        return glfwGetWindowAttrib(*this->window_, GLFW_HOVERED);
    }

    void MouseController::setCursorMode(CursorMode mode)
    {
        glfwSetInputMode(*this->window_, GLFW_CURSOR, mode);
    }

    CursorMode MouseController::cursorMode() const
    {
        i32 mode = glfwGetInputMode(*this->window_, GLFW_CURSOR);
        return static_cast<CursorMode>(mode);
    }

    void MouseController::scrollCallback(GLFWwindow* window, f64 xOffset, f64 yOffset)
    {
        MouseController::scrollDelta_ += vec2(xOffset, yOffset);
    }

    vec2 MouseController::scrollDelta() const
    {
        return MouseController::scrollDelta_;
    }
}