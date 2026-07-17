#pragma once

#include "utilities/vulkan_common.h"

#include <iostream>
#include <cstdlib>

#include "../utility/types.h"
#include "../utility/constants.h"
#include "window.h"
#include "input/input_manager.h"
#include "utilities/queue.h"
#include "geometry/buffer.h"
#include "geometry/texture.h"
#include "geometry/mesh.h"
#include "render_loop/resource_manager.h"
#include "render_loop/render_object.h"
#include "render_loop/game_loop.h"
#include "render_loop/camera.h"

using HelloTriangleApplication::Utilities::FatQueue;
using HelloTriangleApplication::Geometry::FatBuffer;
using HelloTriangleApplication::Geometry::MappedBuffer;
using HelloTriangleApplication::Geometry::Texture;
using HelloTriangleApplication::RenderLoop::ResourceManager;
using HelloTriangleApplication::RenderLoop::GameLoop;
using HelloTriangleApplication::RenderLoop::Camera;
using HelloTriangleApplication::Input::InputManager;

namespace HelloTriangleApplication
{
    class App
    {
    public:
        void run();

        // setup
        Window window;
        InputManager inputs = InputManager(this->window);
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::PhysicalDevice physicalDevice = nullptr;
        vk::raii::Device device = nullptr;
        FatQueue graphicsQueue;
        FatQueue transferQueue;

        // presentation
        vk::raii::SurfaceKHR surface = nullptr;
        vk::raii::SwapchainKHR swapChain = nullptr; // TODO: make FatSwapchain
        std::vector<vk::Image> swapChainImages;
        std::vector<vk::raii::ImageView> swapChainImageViews;
        vk::SurfaceFormatKHR swapChainFormat;
        vk::Extent2D swapChainExtent;
        std::vector<u32> swapChainFamilies;

        // graphics pipeline
        vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::Pipeline graphicsPipeline = nullptr;

        // drawing
        std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
        std::vector<vk::raii::Fence> inFlightFences;
        u32 frameIndex = 0;

        // geometry
        std::vector<MappedBuffer> uniformBuffers = {};
        vk::raii::DescriptorPool descriptorPool = nullptr;
        std::vector<vk::raii::DescriptorSet> descriptorSets = {};
        Texture depthTexture;
        mat4 projection;

        // render loop
        ResourceManager resources;
        GameLoop loop = GameLoop(*this);
        Camera camera;

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
    };
}