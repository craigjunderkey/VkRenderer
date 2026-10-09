#pragma once

#include "utilities/vulkan_common.h"

#include <iostream>
#include <cstdlib>

#include "../utility/types.h"
#include "../utility/constants.h"
#include "config.h"
#include "window.h"
#include "input/input_manager.h"
#include "utilities/queue.h"
#include "utilities/pipeline_builder.h"
#include "utilities/vulkan_memory_helper.h"
#include "utilities/fat_swapchain.h"
#include "geometry/buffer.h"
#include "geometry/texture.h"
#include "geometry/mesh.h"
#include "render_loop/render_object.h"
#include "render_loop/game_loop.h"
#include "render_loop/camera_controller.h"
#include "render_loop/light.h"
#include "render_loop/sampler_manager.h"
#include "render_loop/frame_recorder.h"
#include "resources/resource_manager.h"
#include "resources/vulkan_resources/resource_factory.h"
#include "particles/particle_system.h"

using VkRenderer::Utilities::FatQueue;
using VkRenderer::Utilities::FatPipeline;
using VkRenderer::Utilities::FatSwapchain;
using VkRenderer::Utilities::VulkanMemoryHelper;
using VkRenderer::Geometry::FatBuffer;
using VkRenderer::Geometry::MappedBuffer;
using VkRenderer::Geometry::Texture;
using VkRenderer::RenderLoop::GameLoop;
using VkRenderer::RenderLoop::CameraController;
using VkRenderer::RenderLoop::SamplerManager;
using VkRenderer::RenderLoop::FrameRecorder;
using VkRenderer::Input::InputManager;
using VkRenderer::Resources::ResourceManager;
using VkRenderer::Resources::VulkanResourceFactory;

namespace VkRenderer
{
    class App
    {
    public:
        App(Config config);
        static Config CONFIG;
        void run();

        // setup
        Window window;
        InputManager inputs = InputManager(this->window);
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::PhysicalDevice physicalDevice = nullptr;
        vk::raii::Device device = nullptr;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
        FatQueue graphicsQueue;
        FatQueue transferQueue;
        VulkanMemoryHelper memoryHelper = VulkanMemoryHelper(this->physicalDevice, this->device);
        VulkanResourceFactory vkResFactory = VulkanResourceFactory(
            VkRenderer::Resources::VulkanResourceContext {
                .instance = this->instance,
                .device = this->device,
                .physicalDevice = this->physicalDevice
            }
        );
        std::vector<vk::raii::CommandBuffer> computeCmdBuffers;

        // presentation
        vk::raii::SurfaceKHR surface = nullptr;
        FatSwapchain swapchain;

        // graphics pipeline
        vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
        StringIdMap<FatPipeline> pipelines { "pipelines" };

        // drawing
        std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
        std::vector<vk::raii::Fence> renderFinishedFences;
        u32 frameIndex = 0;

        // geometry
        std::vector<MappedBuffer> uniformBuffers = {};
        vk::raii::DescriptorPool descriptorPool = nullptr;
        std::vector<vk::raii::DescriptorSet> descriptorSets = {};
        Texture depthTexture;
        mat4 projection;
        mat4 view;

        // render loop
        ResourceManager resources = ResourceManager(*this);
        SamplerManager samplers = SamplerManager();
        GameLoop loop = GameLoop(*this);
        CameraController camera = CameraController(RenderLoop::Camera(), this->inputs);
        Environment environment = Environment();
        FrameRecorder frameRecorder = FrameRecorder();

        // post-processing
        vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;
        Texture colorTexture;

        // particles
        Particles::ParticleSystem particleSystem = {};

    private:
        void initLogger();
        void initResources();
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
    };
}