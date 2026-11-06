#include "../vk_renderer.h"
#include "instance.h"

using VkRenderer::App;

Result<std::vector<const char*>, ErrorMessage> fetchExtensions(App& app)
{
    // get required instance extensions from GLFW
    u32 glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    // set required extensions
    std::vector<const char*> extensions;
    for (u32 i = 0; i < glfwExtensionCount; i++)
    {
        extensions.push_back(glfwExtensions[i]);
    }
    
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);

    // check if all the required GLFW extensions are supported
    std::vector<vk::ExtensionProperties> extensionProperties;
    TRY_ASSIGN(extensionProperties, castVkResult(app.context.enumerateInstanceExtensionProperties()));
    bool missingExtension = false;
    for (u32 i = 0; i < extensions.size(); i++)
    {
        bool extensionNotFound = std::ranges::none_of(
            extensionProperties,
            [extension = extensions[i]] (auto const& extensionProperty)
                { return strcmp(extensionProperty.extensionName, extension) == 0; }
        );

        if (extensionNotFound)
        {
            missingExtension = true;
            err({ .channel = "vulkan" }, "Required extension not supported: {}.", extensions[i]);
        }
    }

    if (missingExtension)
        return ErrorMessage();

    return std::move(extensions);
}

Result<std::vector<const char*>, ErrorMessage> fetchValidationLayers(App& app)
{
    // get the required layers
    std::vector<const char*> needLayers;
    if (VALIDATION_LAYERS_ENABLED)
        needLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());

    // check that all the required layers are supported
    auto layerPropertiesResult = castVkResult(app.context.enumerateInstanceLayerProperties());
    CHECK_KEEP(layerPropertiesResult);
    std::vector<vk::LayerProperties> haveLayers = layerPropertiesResult.take();

    for (const char* needLayer : needLayers)
    {
        bool foundIt = false;
        for (vk::LayerProperties haveLayer : haveLayers)
        {
            foundIt |= strncmp(needLayer, haveLayer.layerName, 256) == 0;
        }
        if (!foundIt)
        {
            return ErrorMessage({ .channel = "vulkan" }, "Required validation layer not supported: {}.", needLayer);
        }
    }
    
    return std::move(needLayers);
}

namespace VkRenderer::Utilities
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
        std::vector<const char*> extensions = extensionsResult.take();

        // fetch the validation layers
        auto validationLayersResult = fetchValidationLayers(app);
        CHECK_KEEP(validationLayersResult);
        std::vector<const char*> layers = validationLayersResult.take();

        // configure the instance
        vk::InstanceCreateInfo createInfo {
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = static_cast<u32>(layers.size()),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = static_cast<u32>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

        // enable sync validation layer
        vk::ValidationFeatureEnableEXT syncValidation = vk::ValidationFeatureEnableEXT::eSynchronizationValidation;
        vk::ValidationFeaturesEXT syncValidationLayer {
            .enabledValidationFeatureCount = 1,
            .pEnabledValidationFeatures = &syncValidation
        };
        createInfo.pNext = &syncValidationLayer;

        // create the instance
        auto createResult = castVkResult(app.context.createInstance(createInfo));
        CHECK_KEEP(createResult);
        app.instance = createResult.take();

        return Ok();
    }
}