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

        GLFWwindow* window;
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
    };
}