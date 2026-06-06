#pragma once

#include "utilities/vulkan_common.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

#include "../utility/types.h"
#include "../utility/constants.h"

namespace HelloTriangleApplication
{
    class App
    {
    public:
        void run();

        // setup
        GLFWwindow* window;
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::PhysicalDevice physicalDevice = nullptr;
        vk::raii::Device device = nullptr;
        vk::raii::Queue graphicsQueue = nullptr;

        // presentation
        vk::raii::SurfaceKHR surface = nullptr;
        vk::raii::SwapchainKHR swapChain = nullptr;
        std::vector<vk::Image> swapChainImages;
        std::vector<vk::ImageView> swapChainImageViews;
        vk::SurfaceFormatKHR swapChainFormat;
        vk::Extent2D swapChainExtent;

        // graphics pipeline
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::Pipeline graphicsPipeline = nullptr;

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
    };
}