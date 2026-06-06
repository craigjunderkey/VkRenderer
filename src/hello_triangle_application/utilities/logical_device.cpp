#include "../hello_triangle_application.h"
#include "logical_device.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createLogicalDevice(App& app)
    {
        // get index of first queue family that supports graphics
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = app.physicalDevice.getQueueFamilyProperties();
        
        auto graphicsQueueFamilyProperty = std::ranges::find_if(
            queueFamilyProperties,
            [](const vk::QueueFamilyProperties& properties) {
                return (bool)(properties.queueFlags & vk::QueueFlagBits::eGraphics);
            }
        );
        u32 graphicsIndex = std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty);

        // make device queue
        f32 queuePriority = 0.5f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo {
            .queueFamilyIndex = graphicsIndex, .queueCount = 1, .pQueuePriorities = &queuePriority
        };

        // specify used device features
        vk::PhysicalDeviceFeatures deviceFeatures;
        vk::StructureChain<vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain =
        {
            {},                                 // vk::PhysicalDeviceFeatures2 (empty for now)
            {.shaderDrawParameters = true},     // enable shader draw parameters from Vulkan 1.1
            {.dynamicRendering = true},         // enable dynamic rendering from Vulkan 1.3
            {.extendedDynamicState = true}      // enable extended dynamic state from the extension
        };

        // make logical device
        vk::DeviceCreateInfo deviceCreateInfo = {
            .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueCreateInfo,
            .enabledExtensionCount = (u32)REQUIRED_DEVICE_EXTENSIONS.size(),
            .ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS.data()
        };

        TRY_ASSIGN(
            app.device,
            castVkResult(app.physicalDevice.createDevice(deviceCreateInfo))
        );

        app.graphicsQueue = app.device.getQueue(graphicsIndex, 0);

        return Ok();
    }
}