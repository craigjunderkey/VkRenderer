#pragma once
#include "../window.h"
#include "../utilities/common.h"

namespace HelloTriangleApplication::Input
{
    enum MouseButton
    {
        Left,
        Right,
        Middle,
        Count
    };

    enum CursorMode
    {
        Default = GLFW_CURSOR_NORMAL,
        Locked = GLFW_CURSOR_DISABLED,
        Hidden = GLFW_CURSOR_HIDDEN
    };

    class MouseController
    {
        friend class InputManager;

    private:
        Window& window_;
        std::array<bool, MouseButton::Count> mouseButtons_;
        std::array<bool, MouseButton::Count> prevMouseButtons_;
        vec2 prevPos_;

        void startFrame();
        void endFrame();

    public:
        MouseController(Window& window) : window_(window)
        {
            this->mouseButtons_ = { false };
            this->prevMouseButtons_ = { false };
        }

        bool pressing(MouseButton button) const;
        bool pressed(MouseButton button) const;
        bool released(MouseButton button) const;
        vec2 pos() const;
        vec2 delta() const;
        bool insideWindow() const;

        void setCursorMode(CursorMode mode);
        CursorMode cursorMode() const;
    };
}

using HelloTriangleApplication::Input::MouseButton;
using HelloTriangleApplication::Input::CursorMode;
