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
#include "geometry/color_resources.h"
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
        this->resources.prepareResources(*this, "src/assets.json").unwrap();
        createColorTexture(*this).unwrap();
        createDepthTexture(*this).unwrap();
        createGraphicsPipeline(*this).unwrap();
        createCommandPools(*this).unwrap();
        this->resources.loadResources(*this).unwrap();
        createUniformBuffers(*this).unwrap();
        createDescriptorPool(*this).unwrap();
        createDescriptorSets(*this).unwrap();
        createCommandBuffers(*this).unwrap();
        createSyncObjects(*this).unwrap();
        createProjectionTransform(*this);
    }

    void App::mainLoop()
    {
        std::cout << "Entering main loop\n";
        
        this->loop.initialize();
        this->camera.registerControls();

        u64 startTime = 0;

        while (!this->window.shouldClose())
        {
            u64 nowTime = glfwGetTimerValue();
            
            glfwPollEvents();
            
            // continue if the frame time hasn't elapsed
            f32 timeElapsed = ((nowTime - startTime) / (f64)glfwGetTimerFrequency()) * 1000.0f;
            if (timeElapsed < FRAME_TIME_MS) continue;

            // start of frame
            startTime = nowTime;
            this->inputs.startFrame();
            this->loop.startFrame();
            this->camera.update(FRAME_TIME_SEC);
            this->loop.update(FRAME_TIME_SEC);
            drawFrame(*this, this->loop).unwrap();

            // end of frame
            this->loop.endFrame();
            this->inputs.endFrame();
        }

        this->device.waitIdle();
    }

    void App::cleanup()
    {
        std::cout << "Cleaning up\n";

        glfwTerminate();
    }
}