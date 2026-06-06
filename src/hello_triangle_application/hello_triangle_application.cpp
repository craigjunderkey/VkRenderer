#include "hello_triangle_application.h"
#include "utilities/instance.h"
#include "utilities/physical_device.h"
#include "utilities/logical_device.h"
#include "utilities/surface.h"
#include "utilities/swap_chain.h"
#include "utilities/image_views.h"
#include "utilities/graphics_pipeline.h"
#include "utilities/command_pool.h"
#include "utilities/command_buffer.h"

using namespace HelloTriangleApplication::Utilities;

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

        createInstance(*this).unwrap();
        createSurface(*this).unwrap();
        pickPhysicalDevice(*this).unwrap();
        createLogicalDevice(*this).unwrap();
        createSwapChain(*this).unwrap();
        createImageViews(*this).unwrap();
        createGraphicsPipeline(*this).unwrap();
        createCommandPool(*this).unwrap();
        createCommandBuffer(*this).unwrap();

        recordCommandBuffer(*this, 0);
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