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
#include "resources/prepare_app_resources.h"
#include "resources/image_data.h"

#define ENABLE_STATIC_LOGGING
#include "../utility/log.h"

using namespace HelloTriangleApplication::Utilities;
using namespace HelloTriangleApplication::Geometry;
using namespace HelloTriangleApplication::RenderLoop;
using namespace HelloTriangleApplication::Resources;

namespace HelloTriangleApplication
{
    RuntimeConfig App::RUNTIME_CONFIG {};
    App::App(RuntimeConfig config)
    {
        RUNTIME_CONFIG = config;
    }
    
    void App::run()
    {
        initLogger();
        initResources();
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    void App::initLogger()
    {
        logger = new Log::Logger(
            std::make_shared<Log::LoggingContext>(Log::LoggingContext(
                2,
                { "engine", "resources", "vulkan", "game_loop" }
            )),
            std::make_unique<Log::ConsoleOutput>()
        );
    }

    void App::initResources()
    {
        initializeResourceManager(*this).unwrap();
        handleAssetBuild(*this).unwrap();
        this->resources.registry().startup().unwrap();
        loadAllResources(*this).unwrap();
    }

    void App::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        this->window.attachGlfwHandle(
            glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr)
        );
    }

    void App::initVulkan()
    {
        log("engine", 0, "Initializing Vulkan");

        createInstance(*this).unwrap();
        createSurface(*this).unwrap();
        pickPhysicalDevice(*this).unwrap();
        createLogicalDevice(*this).unwrap();
        createSwapChain(*this).unwrap();
        createImageViews(*this).unwrap();
        createDescriptorSetLayout(*this).unwrap();
        createCommandPools(*this).unwrap();
        createCommandBuffers(*this).unwrap();
        addBindlessResources(*this).unwrap();
        handlePrePipelineInitialization(*this).unwrap();
        createColorTexture(*this).unwrap();
        createDepthTexture(*this).unwrap();
        createGraphicsPipeline(*this).unwrap();
        handlePostLoadInitialization(*this).unwrap();
        loadAndSubmitBindlessResources(*this).unwrap();
        createUniformBuffers(*this).unwrap();
        createDescriptorPool(*this).unwrap();
        createDescriptorSets(*this).unwrap();
        createSyncObjects(*this).unwrap();
        createProjectionTransform(*this);
    }

    void App::mainLoop()
    {
        log("engine", 0, "Entering main loop");
        
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
            this->view = this->camera.handle.view();
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
        log("engine", 0, "Cleaning up");

        this->resources.registry().shutdown().unwrap();
        glfwTerminate();
    }
}