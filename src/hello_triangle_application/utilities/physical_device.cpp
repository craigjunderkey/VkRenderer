#include "../hello_triangle_application.h"
#include "physical_device.h"

using HelloTriangleApplication::App;

bool isDeviceSuitable(const vk::raii::PhysicalDevice& device)
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
    std::vector<const char*> requiredDeviceExtensions = { vk::KHRSwapchainExtensionName };
    auto availableDeviceExtensions = castVkResult(device.enumerateDeviceExtensionProperties()).unwrap();
    bool supportsAllRequiredExtensions =
        std::ranges::all_of(
            requiredDeviceExtensions,
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
        auto physicalDevices = castVkResult(app.instance.enumeratePhysicalDevices()).unwrap();

        if (physicalDevices.empty())
            return Err<std::string>("Failed to find GPUs with Vulkan support.");

        auto deviceIter = std::ranges::find_if(
            physicalDevices,
            [](const vk::raii::PhysicalDevice& physicalDevice) {
                return isDeviceSuitable(physicalDevice);
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