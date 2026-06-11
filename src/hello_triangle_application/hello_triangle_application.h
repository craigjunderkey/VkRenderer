#pragma once

#include "utilities/vulkan_common.h"

#include <iostream>
#include <cstdlib>

#include "../utility/types.h"
#include "../utility/constants.h"
#include "window.h"

namespace HelloTriangleApplication
{
    class App
    {
    public:
        void run();

        // setup
        Window window;
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::PhysicalDevice physicalDevice = nullptr;
        vk::raii::Device device = nullptr;
        Option<u32> queueIndex;
        vk::raii::Queue queue = nullptr;

        // presentation
        vk::raii::SurfaceKHR surface = nullptr;
        vk::raii::SwapchainKHR swapChain = nullptr;
        std::vector<vk::Image> swapChainImages;
        std::vector<vk::raii::ImageView> swapChainImageViews;
        vk::SurfaceFormatKHR swapChainFormat;
        vk::Extent2D swapChainExtent;

        // graphics pipeline
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::Pipeline graphicsPipeline = nullptr;

        // drawing
        vk::raii::CommandPool commandPool = nullptr;
        std::vector<vk::raii::CommandBuffer> commandBuffers;
        std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
        std::vector<vk::raii::Fence> inFlightFences;
        u32 frameIndex = 0;

        // geometry
        vk::raii::Buffer vertexBuffer = nullptr;
        vk::raii::DeviceMemory vertexBufferMemory = nullptr;

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
    };
}