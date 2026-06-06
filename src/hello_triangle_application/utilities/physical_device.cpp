#include "../hello_triangle_application.h"
#include "physical_device.h"

using HelloTriangleApplication::App;

Result<bool, std::string> isDeviceSuitable(const vk::raii::PhysicalDevice& device)
{
    // supports Vulkan 1.3
    auto properties = device.getProperties();
    bool supportsVulkan1_3 = properties.apiVersion >= vk::ApiVersion13;
    if (!supportsVulkan1_3) return false;

    // supports graphics
    auto queueFamilies = device.getQueueFamilyProperties();
    bool supportsGraphics =
        std::ranges::any_of(
            queueFamilies,
            [](const vk::QueueFamilyProperties& properties)
                { return (bool)(properties.queueFlags & vk::QueueFlagBits::eGraphics); }
        );
    if (!supportsGraphics) return false;

    // supports all extensions
    std::vector<vk::ExtensionProperties> availableDeviceExtensions;
    TRY_ASSIGN(availableDeviceExtensions, castVkResult(device.enumerateDeviceExtensionProperties()));

    bool supportsAllRequiredExtensions =
        std::ranges::all_of(
            REQUIRED_DEVICE_EXTENSIONS,
            [&availableDeviceExtensions](const char* requiredExtension) {
                return std::ranges::any_of(
                    availableDeviceExtensions,
                    [requiredExtension](const vk::ExtensionProperties& extension) {
                        return strcmp((const char*)extension.extensionName, requiredExtension) == 0;
                    }
                );
            }
        );
    if (!supportsAllRequiredExtensions) return false;

    // supports required features
    auto features = device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsAllRequiredFeatures =
        features.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
        features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
        features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;
    if (!supportsAllRequiredFeatures) return false;

    return true;
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> pickPhysicalDevice(App& app)
    {
        std::vector<vk::raii::PhysicalDevice> physicalDevices;
        TRY_ASSIGN(physicalDevices, castVkResult(app.instance.enumeratePhysicalDevices()));

        if (physicalDevices.empty())
            return Err<std::string>("Failed to find GPUs with Vulkan support.");

        auto deviceIter = std::ranges::find_if(
            physicalDevices,
            [](const vk::raii::PhysicalDevice& physicalDevice) {
                Result<bool, std::string> suitability = isDeviceSuitable(physicalDevice);
                return suitability.isOk() && suitability.value();
            }
        );

        if (deviceIter == physicalDevices.end())
        {
            return Err<std::string>("Failed to find a suitable GPU.");
        }

        app.physicalDevice = *deviceIter;

        return Ok();
    }
}