#include <vulkan/vulkan.hpp>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define VULKAN_HPP_NO_EXCEPTIONS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include "types.h"

constexpr u32 WIDTH = 1600;
constexpr u32 HEIGHT = 1200;
constexpr const char* TITLE = "HOUSE OF HENRY";

class HelloTriangleApplication
{
    public:
        void run()
        {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:
        GLFWwindow* window;
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;

        void initWindow()
        {
            std::cout << "Initializing window\n";
            
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            this->window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
        }
        
        void initVulkan()
        {
            std::cout << "Initializing Vulkan\n";

            createInstance().unwrap();
        }

        void mainLoop()
        {
            std::cout << "Entering main loop\n";
            
            while (!glfwWindowShouldClose(this->window))
            {
                glfwPollEvents();
            }
        }
        
        void cleanup()
        {
            std::cout << "Cleaning up\n";

            glfwDestroyWindow(this->window);
            glfwTerminate();
        }

        Result<Unit, std::string> createInstance()
        {
            
            constexpr vk::ApplicationInfo appInfo {
                .pApplicationName = TITLE,
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "No Engine",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = vk::ApiVersion14
            };

            // get required instance extensions from GLFW
            u32 glfwExtensionCount = 0;
            auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            // check if all the required GLFW extensions are supported
            auto extensionProperties = context.enumerateInstanceExtensionProperties();
            for (u32 i = 0; i < glfwExtensionCount; i++)
            {
                bool extensionNotFound = std::ranges::none_of(
                    extensionProperties,
                    [glfwExtension = glfwExtensions[i]] (auto const& extensionProperty)
                        { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }
                );

                if (extensionNotFound)
                    return Err<std::string>("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
                
            }

            vk::InstanceCreateInfo createInfo {
                .pApplicationInfo = &appInfo,
                .enabledExtensionCount = glfwExtensionCount,
                .ppEnabledExtensionNames = glfwExtensions
            };

            this->instance = vk::raii::Instance(this->context, createInfo);

            return Ok();
        }
};

int main()
{
    HelloTriangleApplication app;
    app.run();
    return EXIT_SUCCESS;
}