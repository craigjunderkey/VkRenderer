#include "window.h"

namespace HelloTriangleApplication
{
    Window::Window(GLFWwindow* handle)
    {
        this->attachGlfwHandle(handle);
    }

    Window::~Window()
    {
        glfwDestroyWindow(this->handle);
    }

    GLFWwindow* Window::operator*()
    {
        return this->handle;
    }

    void Window::attachGlfwHandle(GLFWwindow* handle)
    {
        assert(this->handle == nullptr);
        assert(handle != nullptr);

        this->handle = handle;

        glfwSetWindowUserPointer(handle, &this->status);
        glfwSetFramebufferSizeCallback(handle, framebufferResizeCallback);
        glfwSetWindowIconifyCallback(handle, iconifyCallback);
    }

    void Window::framebufferResizeCallback(GLFWwindow* handle, i32 width, i32 height)
    {
        auto status = reinterpret_cast<Window::Status*>(glfwGetWindowUserPointer(handle));
        status->framebufferResized = true;
    }

    void Window::iconifyCallback(GLFWwindow* handle, i32 isIconified)
    {
        auto status = reinterpret_cast<Window::Status*>(glfwGetWindowUserPointer(handle));
        status->isMinimized = isIconified == GLFW_TRUE;
    }
}