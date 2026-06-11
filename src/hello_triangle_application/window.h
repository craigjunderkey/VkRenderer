#include <GLFW/glfw3.h>
#include "../utility/types.h"
#include "../utility/constants.h"

namespace HelloTriangleApplication
{
    class Window
    {
    public:
        struct Status
        {
            bool framebufferResized = false;
            bool isMinimized = false;
        };
    
        GLFWwindow* handle = nullptr;
        Status status;

        Window() = default;
        Window(GLFWwindow* handle);
        ~Window();

        GLFWwindow* operator*();
        void attachGlfwHandle(GLFWwindow* handle);

    private:
        static void framebufferResizeCallback(GLFWwindow* handle, i32 width, i32 height);
        static void iconifyCallback(GLFWwindow* handle, i32 isIconified);
    };
}