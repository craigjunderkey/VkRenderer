#pragma once
#include "../window.h"
#include "../utilities/common.h"

namespace VkRenderer::Input
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
        bool hasRegisteredCallback_;

        void startFrame();
        void endFrame();

        static vec2 scrollDelta_;
        static void scrollCallback(GLFWwindow* window, f64 xOffset, f64 yOffset);

    public:
        MouseController(Window& window);

        bool pressing(MouseButton button) const;
        bool pressed(MouseButton button) const;
        bool released(MouseButton button) const;
        vec2 pos() const;
        vec2 delta() const;
        vec2 scrollDelta() const;
        bool insideWindow() const;

        void setCursorMode(CursorMode mode);
        CursorMode cursorMode() const;
    };
}

using VkRenderer::Input::MouseButton;
using VkRenderer::Input::CursorMode;
