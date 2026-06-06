#include "hello_triangle_application.h"
#include "utilities/instance.h"
#include "utilities/physical_device.h"

namespace HelloTriangleApplication
{
    void App::run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    void App::initWindow()
    {
        std::cout << "Initializing window\n";
        
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        this->window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
    }

    void App::initVulkan()
    {
        std::cout << "Initializing Vulkan\n";

        Utilities::createInstance(*this).unwrap();
        Utilities::pickPhysicalDevice(*this).unwrap();
    }

    void App::mainLoop()
    {
        std::cout << "Entering main loop\n";
        
        while (!glfwWindowShouldClose(this->window))
        {
            glfwPollEvents();
        }
    }

    void App::cleanup()
    {
        std::cout << "Cleaning up\n";

        glfwDestroyWindow(this->window);
        glfwTerminate();
    }
}