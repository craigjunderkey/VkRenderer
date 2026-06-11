#pragma once

#include "utilities/vulkan_common.h"

#include <iostream>
#include <cstdlib>

#include "../utility/types.h"
#include "../utility/constants.h"
#include "window.h"
#include "utilities/queue.h"

using HelloTriangleApplication::Utilities::FatQueue;

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
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::Pipeline graphicsPipeline = nullptr;

        // drawing
        std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
        std::vector<vk::raii::Fence> inFlightFences;
        u32 frameIndex = 0;

        // geometry
        vk::raii::Buffer vertexBuffer = nullptr;
        vk::raii::DeviceMemory vertexBufferMemory = nullptr;
        std::vector<u32> vertexBufferFamilies;

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
    };
}