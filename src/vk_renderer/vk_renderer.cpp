#include "vk_renderer.h"
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
#include "utilities/vulkan_debug_output.h"
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
#include "particles/init_particles.h"

#include "../utility/log/log.h"
#include "../utility/log/log_utils.h"
#include "../utility/log/log_output.h"
#include "../utility/log/log_error.h"

using namespace VkRenderer::Utilities;
using namespace VkRenderer::Geometry;
using namespace VkRenderer::RenderLoop;
using namespace VkRenderer::Resources;
using namespace VkRenderer::Particles;
using namespace LogUtils;

namespace VkRenderer
{
    Config App::CONFIG {};
    App::App(Config config)
    {
        CONFIG = config;
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
        auto logContext = std::make_shared<Log::LoggingContext>(
            Log::LoggingContext(
                2,
                { "engine", "resources", "vulkan", "game_loop" }
            )
        );
        setGlobalLogger(new Log::Logger(
            logContext,
            std::make_unique<Log::ConsoleOutput>()
        ));
        getGlobalLogger().forwardTo(
            std::make_unique<Log::Logger>(
                Log::Logger(
                    logContext,
                    std::make_unique<Log::FileOutput>(Log::FileOutput("log"))
                )
            )
        );
        getGlobalLogger().setVerbosity(1);
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
        log({ .channel = "engine" }, "Initializing Vulkan");

        log({ .channel = "vulkan", .verbosity = 2 }, "createInstance");
        createInstance(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createDebugMessenger");
        createDebugMessenger(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createSurface");
        createSurface(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "pickPhysicalDevice");
        pickPhysicalDevice(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createLogicalDevice");
        createLogicalDevice(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createSwapChain");
        createSwapChain(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createDescriptorSetLayout");
        createDescriptorSetLayout(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createCommandPools");
        createCommandPools(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createCommandBuffers");
        createCommandBuffers(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "addBindlessResources");
        addBindlessResources(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "handlePrePipelineInitialization");
        handlePrePipelineInitialization(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createColorTexture");
        createColorTexture(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createDepthTexture");
        createDepthTexture(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createGraphicsPipeline");
        createGraphicsPipeline(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "handlePostLoadInitialization");
        handlePostLoadInitialization(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "loadAndSubmitBindlessResources");
        loadAndSubmitBindlessResources(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2}, "createParticleSystem");
        particleSystem = createParticleSystem(*this, 1000).unwrap();
        log({ .channel = "vulkan", .verbosity = 2}, "createParticleComputePipeline");
        pipelines.add("particle_compute", createParticleComputePipeline(*this).unwrap());
        log({ .channel = "vulkan", .verbosity = 2 }, "createPerFrameUbos");
        createPerFrameUbos(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createDescriptorPool");
        createDescriptorPool(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createDescriptorSets");
        createDescriptorSets(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createSyncObjects");
        createSyncObjects(*this).unwrap();
        log({ .channel = "vulkan", .verbosity = 2 }, "createProjectionTransform");
        createProjectionTransform(*this);
    }

    void App::mainLoop()
    {
        log({ .channel = "engine" }, "Entering main loop");
        
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
        log({ .channel = "engine" }, "Cleaning up");

        this->resources.registry().shutdown().unwrap();
        glfwTerminate();
    }
}