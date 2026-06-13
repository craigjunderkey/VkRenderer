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
#include "utilities/sync_objects.h"
#include "utilities/draw.h"
#include "geometry/vertex_buffer.h"
#include "geometry/descriptor_set_layout.h"
#include "geometry/uniform_buffer.h"
#include "geometry/descriptor_pool.h"
#include "geometry/texture.h"

using namespace HelloTriangleApplication::Utilities;
using namespace HelloTriangleApplication::Geometry;

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
        this->window.attachGlfwHandle(
            glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr)
        );
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
        createDescriptorSetLayout(*this).unwrap();
        createGraphicsPipeline(*this).unwrap();
        createCommandPools(*this).unwrap();
        this->texture = Texture::load(*this, TEXTURE_PATH).unwrap();
        this->texture.makeSampler(*this).unwrap();
        createBuffers(*this).unwrap();
        createUniformBuffers(*this).unwrap();
        createDescriptorPool(*this).unwrap();
        createDescriptorSets(*this).unwrap();
        createCommandBuffers(*this).unwrap();
        createSyncObjects(*this).unwrap();
    }

    void App::mainLoop()
    {
        std::cout << "Entering main loop\n";

        while (!glfwWindowShouldClose(*this->window))
        {
            glfwPollEvents();
            drawFrame(*this).unwrap();
        }

        this->device.waitIdle();
    }

    void App::cleanup()
    {
        std::cout << "Cleaning up\n";

        glfwTerminate();
    }
}