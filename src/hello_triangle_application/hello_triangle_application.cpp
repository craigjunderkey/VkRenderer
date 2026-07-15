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
#include "geometry/depth_resources.h"
#include "geometry/mesh.h"
#include "render_loop/transform.h"

using namespace HelloTriangleApplication::Utilities;
using namespace HelloTriangleApplication::Geometry;
using namespace HelloTriangleApplication::RenderLoop;

namespace HelloTriangleApplication
{
    void App::run()
    {
        initWindow();
        initResources();
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

    void App::initResources()
    {
        std::cout << "Initializing resources\n";
        this->resources.prepareResources(*this, "src/assets.json").unwrap();
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
        createDepthTexture(*this).unwrap();
        createGraphicsPipeline(*this).unwrap();
        createCommandPools(*this).unwrap();
        this->resources.loadResources(*this).unwrap();
        createUniformBuffers(*this).unwrap();
        createDescriptorPool(*this).unwrap();
        createDescriptorSets(*this).unwrap();
        createCommandBuffers(*this).unwrap();
        createSyncObjects(*this).unwrap();
        createViewTransform(*this);
        createProjectionTransform(*this);
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