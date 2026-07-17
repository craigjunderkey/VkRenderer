#include "mouse_controller.h"

namespace HelloTriangleApplication::Input
{
    void MouseController::startFrame()
    {
        // update mouse buttons
        this->mouseButtons_[MouseButton::Left] = glfwGetMouseButton(*this->window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        this->mouseButtons_[MouseButton::Right] = glfwGetMouseButton(*this->window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        this->mouseButtons_[MouseButton::Middle] = glfwGetMouseButton(*this->window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
    }

    void MouseController::endFrame()
    {
        this->prevMouseButtons_.swap(this->mouseButtons_);
        this->prevPos_ = this->pos();
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
}