#include "../hello_triangle_application.h"
#include "instance.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createInstance(App& app)
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
        auto extensionProperties = app.context.enumerateInstanceExtensionProperties();
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

        // create the instance
        vk::InstanceCreateInfo createInfo {
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = glfwExtensionCount,
            .ppEnabledExtensionNames = glfwExtensions
        };

        app.instance = vk::raii::Instance(app.context, createInfo);

        // get the required layers
        std::vector<const char*> requiredLayers;
        if (VALIDATION_LAYERS_ENABLED)
            requiredLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());

        // check that all the required layers are supported
        auto layerProperties = app.context.enumerateInstanceLayerProperties();
        auto unsupportedLayerIt = std::ranges::find_if(
            layerProperties,
            [requiredLayers] (vk::LayerProperties layer) {
                return std::ranges::contains(requiredLayers, (const char*)layer.layerName);
            }
        );
        if (unsupportedLayerIt != layerProperties.end())
            return Err<std::string>("Required validation layer not supported: " + std::string(unsupportedLayerIt->layerName));

        return Ok();
    }
}