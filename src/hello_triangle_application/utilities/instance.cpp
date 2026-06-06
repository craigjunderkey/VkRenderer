#include "../hello_triangle_application.h"
#include "instance.h"

using HelloTriangleApplication::App;

struct Extensions
{
    const char** extensionNames;
    u32 count;
};

Result<Extensions, std::string> fetchExtensions(App& app)
{
    // get required instance extensions from GLFW
    u32 glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // check if all the required GLFW extensions are supported
    std::vector<vk::ExtensionProperties> extensionProperties;
    TRY_ASSIGN(extensionProperties, castVkResult(app.context.enumerateInstanceExtensionProperties()));
    
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

    return Extensions { .extensionNames = glfwExtensions, .count = glfwExtensionCount };
}

struct ValidationLayers
{
    std::vector<const char*> layerNames;
    u32 count;
};

Result<ValidationLayers, std::string> fetchValidationLayers(App& app)
{
    // get the required layers
    std::vector<const char*> requiredLayers;
    if (VALIDATION_LAYERS_ENABLED)
        requiredLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());

    // check that all the required layers are supported
    std::vector<vk::LayerProperties> layerProperties;
    TRY_ASSIGN(layerProperties, castVkResult(app.context.enumerateInstanceLayerProperties()));

    auto unsupportedLayerIt = std::ranges::find_if(
        layerProperties,
        [requiredLayers] (vk::LayerProperties layer) {
            return std::ranges::contains(requiredLayers, (const char*)layer.layerName);
        }
    );

    if (unsupportedLayerIt != layerProperties.end())
        return Err<std::string>("Required validation layer not supported: " + std::string(unsupportedLayerIt->layerName));
    
    u32 layerCount = requiredLayers.size();

    return ValidationLayers { .layerNames = std::move(requiredLayers), .count = layerCount };
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createInstance(App& app)
    {
        // describe app
        constexpr vk::ApplicationInfo appInfo {
            .pApplicationName = TITLE,
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = vk::ApiVersion14
        };

        // fetch the extensions
        Extensions extensions;
        TRY_ASSIGN(extensions, fetchExtensions(app));

        // fetch the validation layers
        ValidationLayers layers;
        TRY_ASSIGN(layers, fetchValidationLayers(app));

        // create the instance
        vk::InstanceCreateInfo createInfo {
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = layers.count,
            .ppEnabledLayerNames = layers.layerNames.data(),
            .enabledExtensionCount = extensions.count,
            .ppEnabledExtensionNames = extensions.extensionNames
        };

        TRY_ASSIGN(app.instance, castVkResult(app.context.createInstance(createInfo)));

        return Ok();
    }
}