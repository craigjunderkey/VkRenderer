#include "../vk_renderer.h"
#include "logical_device.h"

using VkRenderer::App;

Result<u32, ErrorMessage> getGraphicsFamilyIndex(App& app, const std::vector<vk::QueueFamilyProperties>& families)
{
    // get index of first queue family that supports graphics and presentation
    Option<u32> index;
    for (u32 i = 0; i < families.size(); i++)
    {
        const vk::QueueFamilyProperties& properties = families[i];
        bool supportsGraphics = static_cast<bool>(properties.queueFlags & vk::QueueFlagBits::eGraphics);
        bool supportsCompute = static_cast<bool>(properties.queueFlags & vk::QueueFlagBits::eCompute);

        Result<u32, ErrorMessage> surfaceSupport = castVkResult(app.physicalDevice.getSurfaceSupportKHR(i, *app.surface));
        bool supportsSurfaces = surfaceSupport.isOk() && surfaceSupport.value() != 0;

        if (supportsGraphics && supportsSurfaces)
        {
            index = i;
            break;
        }
    }

    if (!index)
        return ErrorMessage({ .channel = "vulkan" },
            "Failed to find a queue family that supports graphics, compute, and presenting.");

    return (u32)index.value();
}

Result<u32, ErrorMessage> getTransferFamilyIndex(App& app, const std::vector<vk::QueueFamilyProperties>& families)
{
    // get index of first queue family that supports transfer but not graphics
    Option<u32> index;
    for (u32 i = 0; i < families.size(); i++)
    {
        const vk::QueueFamilyProperties& properties = families[i];
        bool supportsTransfer = static_cast<bool>(properties.queueFlags & vk::QueueFlagBits::eTransfer);
        bool supportsGraphics = static_cast<bool>(properties.queueFlags & vk::QueueFlagBits::eGraphics);

        if (supportsTransfer & !supportsGraphics)
        {
            index = i;
            break;
        }
    }

    if (!index)
        return ErrorMessage({ .channel = "vulkan" }, "Failed to find a queue family that supports transfer but not graphics.");

    return (u32)index.value();
}

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createLogicalDevice(App& app)
    {
        std::vector<vk::QueueFamilyProperties> queueFamilies = app.physicalDevice.getQueueFamilyProperties();
        std::array<vk::DeviceQueueCreateInfo, 2> queueCreateInfos = {};
        f32 queuePriority = 0.5f;

        // prepare graphics queue
        auto graphicsFamilyIndex = getGraphicsFamilyIndex(app, queueFamilies);
        CHECK_KEEP(graphicsFamilyIndex);
        queueCreateInfos[0] = vk::DeviceQueueCreateInfo {
            .queueFamilyIndex = graphicsFamilyIndex.value(), .queueCount = 1, .pQueuePriorities = &queuePriority
        };

        // prepare transfer queue
        auto transferFamilyIndex = getTransferFamilyIndex(app, queueFamilies);
        CHECK_KEEP(transferFamilyIndex);
        queueCreateInfos[1] = vk::DeviceQueueCreateInfo {
            .queueFamilyIndex = transferFamilyIndex.value(), .queueCount = 1, .pQueuePriorities = &queuePriority
        };

        // specify used device features
        vk::PhysicalDeviceFeatures deviceFeatures;
        vk::StructureChain<vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain =
        {
            {.features = { .samplerAnisotropy = true }},            // vk::PhysicalDeviceFeatures2
            {.shaderDrawParameters = true},                         // enable shader draw parameters from Vulkan 1.1
            {.synchronization2 = true, .dynamicRendering = true},   // enable dynamic rendering from Vulkan 1.3
            {.extendedDynamicState = true}                          // enable extended dynamic state from the extension
        };

        // make logical device
        vk::DeviceCreateInfo deviceCreateInfo = {
            .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = (u32)queueCreateInfos.size(),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledExtensionCount = (u32)REQUIRED_DEVICE_EXTENSIONS.size(),
            .ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS.data()
        };

        TRY_ASSIGN(
            app.device,
            castVkResult(app.physicalDevice.createDevice(deviceCreateInfo))
        );

        // create graphics and transfer queues
        vk::raii::Queue graphicsQueue = app.device.getQueue(graphicsFamilyIndex.value(), 0);
        app.graphicsQueue = FatQueue { .queue = std::move(graphicsQueue), .familyIndex = graphicsFamilyIndex.value() };

        vk::raii::Queue transferQueue = app.device.getQueue(transferFamilyIndex.value(), 0);
        app.transferQueue = FatQueue { .queue = std::move(transferQueue), .familyIndex = transferFamilyIndex.value() };

        return Ok();
    }
}