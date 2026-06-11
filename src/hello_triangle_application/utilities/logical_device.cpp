#include "../hello_triangle_application.h"
#include "logical_device.h"

using HelloTriangleApplication::App;

Result<u32, std::string> getGraphicsFamilyIndex(App& app, const std::vector<vk::QueueFamilyProperties>& families)
{
    // get index of first queue family that supports graphics and presentation
    Option<u32> index;
    for (u32 i = 0; i < families.size(); i++)
    {
        const vk::QueueFamilyProperties& properties = families[i];
        bool supportsGraphics = static_cast<bool>(properties.queueFlags & vk::QueueFlagBits::eGraphics);

        Result<u32, std::string> surfaceSupport = castVkResult(app.physicalDevice.getSurfaceSupportKHR(i, *app.surface));
        bool supportsSurfaces = surfaceSupport.isOk() && surfaceSupport.value() != 0;

        if (supportsGraphics && supportsSurfaces)
        {
            index = i;
            break;
        }
    }

    if (!index) return Err<std::string>("Failed to find a queue family that supports graphics and presenting.");

    return (u32)index.value();
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createLogicalDevice(App& app)
    {
        // get graphics queue index
        std::vector<vk::QueueFamilyProperties> queueFamilies = app.physicalDevice.getQueueFamilyProperties();
        auto graphicsFamilyIndex = getGraphicsFamilyIndex(app, queueFamilies);
        CHECK(graphicsFamilyIndex);

        // prepare graphics queue
        f32 queuePriority = 0.5f;
        vk::DeviceQueueCreateInfo graphicsQueueCreateInfo {
            .queueFamilyIndex = graphicsFamilyIndex.value(), .queueCount = 1, .pQueuePriorities = &queuePriority
        };

        // specify used device features
        vk::PhysicalDeviceFeatures deviceFeatures;
        vk::StructureChain<vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain =
        {
            {},                                                     // vk::PhysicalDeviceFeatures2 (empty for now)
            {.shaderDrawParameters = true},                         // enable shader draw parameters from Vulkan 1.1
            {.synchronization2 = true, .dynamicRendering = true},   // enable dynamic rendering from Vulkan 1.3
            {.extendedDynamicState = true}                          // enable extended dynamic state from the extension
        };

        // make logical device
        vk::DeviceCreateInfo deviceCreateInfo = {
            .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &graphicsQueueCreateInfo,
            .enabledExtensionCount = (u32)REQUIRED_DEVICE_EXTENSIONS.size(),
            .ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS.data()
        };

        TRY_ASSIGN(
            app.device,
            castVkResult(app.physicalDevice.createDevice(deviceCreateInfo))
        );

        vk::raii::Queue graphicsQueue = app.device.getQueue(graphicsFamilyIndex.value(), 0);
        app.graphicsQueue = FatQueue { .queue = std::move(graphicsQueue), .familyIndex = graphicsFamilyIndex.value() };

        return Ok();
    }
}