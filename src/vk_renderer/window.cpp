#include "window.h"
#include "utilities/common.h"

namespace VkRenderer
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

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(this->handle) || this->status.markedForClosure;
    }

    std::pair<u32, u32> Window::getFramebufferSize() const
    {
        i32 width, height;
        glfwGetFramebufferSize(this->handle, &width, &height);
        return std::pair<u32, u32>(static_cast<u32>(width), static_cast<u32>(height));
    }
}