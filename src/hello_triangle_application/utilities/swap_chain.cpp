#include "../hello_triangle_application.h"
#include "swap_chain.h"
#include "image_views.h"
#include "../geometry/depth_resources.h"
#include "../render_loop/transform.h"

using HelloTriangleApplication::App;

Result<const vk::SurfaceFormatKHR, std::string> chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    if (availableFormats.empty()) return Err<std::string>("Surface has no available formats.");
    
    const auto formatIt = std::ranges::find_if(
        availableFormats,
        [] (const vk::SurfaceFormatKHR& format) {
            return (format.format == vk::Format::eR8G8B8A8Srgb || format.format == vk::Format::eB8G8R8A8Srgb)
                && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        }
    );

    bool foundIdealFormat = formatIt != availableFormats.end();

    vk::SurfaceFormatKHR format;
    if (foundIdealFormat)
    {
        format = *formatIt;
    }
    else
    {
        format = availableFormats[0];
    }

    return Result<const vk::SurfaceFormatKHR, std::string>(format);
}

Result<vk::PresentModeKHR, std::string> chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
    bool supportsFifo = std::ranges::any_of(availablePresentModes, [](const auto& mode) { return mode == vk::PresentModeKHR::eFifo; });
    
    bool supportsMailbox = std::ranges::any_of(
        availablePresentModes,
        [] (const auto& mode) { return mode == vk::PresentModeKHR::eMailbox; }
    );
    
    if (supportsMailbox)
    {
        return vk::PresentModeKHR::eMailbox;
    }
    else if (supportsFifo)
    {
        return vk::PresentModeKHR::eFifo;
    }
    else
    {
        return Err<std::string>("Surface supports neither FIFO nor mailbox present modes.");
    }
}

vk::Extent2D chooseSwapExtent(App& app, const vk::SurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
    {
        return capabilities.currentExtent;
    }

    i32 width, height;
    glfwGetFramebufferSize(*app.window, &width, &height);

    return vk::Extent2D {
        .width = std::clamp<u32>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        .height = std::clamp<u32>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

u32 chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    u32 minImageCount = std::max(3u, capabilities.minImageCount);

    // clamp to max image count
    bool hasMax = capabilities.maxImageCount > 0;
    if (hasMax && capabilities.maxImageCount < minImageCount)
    {
        minImageCount = capabilities.maxImageCount;
    }

    return minImageCount;
}

Result<Unit, std::string> cleanupSwapChain(App& app)
{
    app.swapChainImageViews.clear();
    app.swapChain = nullptr;
    return Ok();
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createSwapChain(App& app)
    {
        // collect surface details
        vk::SurfaceCapabilitiesKHR surfaceCapabilities;
        TRY_ASSIGN(surfaceCapabilities, castVkResult(app.physicalDevice.getSurfaceCapabilitiesKHR(*app.surface)));

        std::vector<vk::SurfaceFormatKHR> availableFormats;
        TRY_ASSIGN(availableFormats, castVkResult(app.physicalDevice.getSurfaceFormatsKHR(*app.surface)));

        std::vector<vk::PresentModeKHR> availablePresentModes;
        TRY_ASSIGN(availablePresentModes, castVkResult(app.physicalDevice.getSurfacePresentModesKHR(*app.surface)));

        // choose settings for the swap chain
        vk::SurfaceFormatKHR format;
        TRY_ASSIGN(format, chooseSwapSurfaceFormat(availableFormats));

        vk::PresentModeKHR presentMode;
        TRY_ASSIGN(presentMode, chooseSwapPresentMode(availablePresentModes));

        vk::Extent2D extent = chooseSwapExtent(app, surfaceCapabilities);
        u32 minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

        // decide which queue families can access the swapchain
        app.swapChainFamilies = {
            app.graphicsQueue.familyIndex,
            app.transferQueue.familyIndex
        };
        
        // create the swap chain
        vk::SwapchainCreateInfoKHR swapChainCreateInfo {
            .surface = *app.surface,
            .minImageCount = minImageCount,
            .imageFormat = format.format,
            .imageColorSpace = format.colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode = vk::SharingMode::eConcurrent,
            .queueFamilyIndexCount = (u32)app.swapChainFamilies.size(),
            .pQueueFamilyIndices = app.swapChainFamilies.data(),
            .preTransform = surfaceCapabilities.currentTransform,
            .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode = presentMode,
            .clipped = true,
            .oldSwapchain = nullptr
        };

        if (app.swapChain != VK_NULL_HANDLE)
            swapChainCreateInfo.oldSwapchain = *app.swapChain;

        TRY_ASSIGN(
            app.swapChain,
            castVkResult(app.device.createSwapchainKHR(swapChainCreateInfo))
        );

        // fetch swap chain images
        TRY_ASSIGN(
            app.swapChainImages,
            castVkResult(app.swapChain.getImages())
        );
        

        // store format and extent
        app.swapChainFormat = format;
        app.swapChainExtent = extent;

        return Ok();
    }

    Result<Unit, std::string> recreateSwapChain(App& app)
    {
        while (app.window.status.isMinimized)
        {
            glfwWaitEvents();
        }

        app.device.waitIdle();

        CHECK(cleanupSwapChain(app));
        CHECK(createSwapChain(app));
        CHECK(createImageViews(app));
        CHECK(Geometry::createDepthTexture(app));
        RenderLoop::createProjectionTransform(app);

        return Ok();
    }
}