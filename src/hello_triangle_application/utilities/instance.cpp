#include "../hello_triangle_application.h"
#include "instance.h"

using HelloTriangleApplication::App;

struct Extensions
{
    const char** extensionNames;
    u32 count;
};

Result<Extensions, ErrorMessage> fetchExtensions(App& app)
{
    // get required instance extensions from GLFW
    u32 glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // check if all the required GLFW extensions are supported
    std::vector<vk::ExtensionProperties> extensionProperties;
    TRY_ASSIGN(extensionProperties, castVkResult(app.context.enumerateInstanceExtensionProperties()));
    bool missingExtension = false;
    for (u32 i = 0; i < glfwExtensionCount; i++)
    {
        bool extensionNotFound = std::ranges::none_of(
            extensionProperties,
            [glfwExtension = glfwExtensions[i]] (auto const& extensionProperty)
                { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }
        );

        if (extensionNotFound)
        {
            missingExtension = true;
            err({ .channel = "vulkan" }, "Required GLFW extension not supported: {}.", std::string(glfwExtensions[i]));
        }
    }

    if (missingExtension)
        return ErrorMessage();

    return Extensions { .extensionNames = glfwExtensions, .count = glfwExtensionCount };
}

struct ValidationLayers
{
    std::vector<const char*> layerNames;
    u32 count;
};

Result<ValidationLayers, ErrorMessage> fetchValidationLayers(App& app)
{
    // get the required layers
    std::vector<const char*> requiredLayers;
    if (VALIDATION_LAYERS_ENABLED)
        requiredLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());

    // check that all the required layers are supported
    auto layerPropertiesResult = castVkResult(app.context.enumerateInstanceLayerProperties());
    CHECK_KEEP(layerPropertiesResult);
    std::vector<vk::LayerProperties> layerProperties = layerPropertiesResult.take();

    auto unsupportedLayerIt = std::ranges::find_if(
        layerProperties,
        [requiredLayers] (vk::LayerProperties layer) {
            return std::ranges::contains(requiredLayers, (const char*)layer.layerName);
        }
    );

    if (unsupportedLayerIt != layerProperties.end())
        return ErrorMessage({ .channel = "vulkan" }, "Required validation layer not supported: {}.", unsupportedLayerIt->layerName);
    
    u32 layerCount = requiredLayers.size();

    return ValidationLayers { .layerNames = std::move(requiredLayers), .count = layerCount };
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, ErrorMessage> createInstance(App& app)
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
        auto extensionsResult = fetchExtensions(app);
        CHECK_KEEP(extensionsResult);
        Extensions extensions = extensionsResult.take();

        // fetch the validation layers
        auto validationLayersResult = fetchValidationLayers(app);
        CHECK_KEEP(validationLayersResult);
        ValidationLayers layers = validationLayersResult.take();

        // create the instance
        vk::InstanceCreateInfo createInfo {
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = layers.count,
            .ppEnabledLayerNames = layers.layerNames.data(),
            .enabledExtensionCount = extensions.count,
            .ppEnabledExtensionNames = extensions.extensionNames
        };

        auto createResult = castVkResult(app.context.createInstance(createInfo));
        CHECK_KEEP(createResult);
        app.instance = createResult.take();

        return Ok();
    }
}