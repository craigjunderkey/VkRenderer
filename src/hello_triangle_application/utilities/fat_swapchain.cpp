#include "fat_swapchain.h"
#include "GLFW/glfw3.h"

namespace HelloTriangleApplication::Utilities
{
    FatSwapchainDescription FatSwapchainDescription::makeDefault()
    {
        return FatSwapchainDescription {
            .presentModePriorities = {
                vk::PresentModeKHR::eMailbox,
                vk::PresentModeKHR::eFifo
            },
            .surfaceFormatPriorities = {
                vk::SurfaceFormatKHR {
                    .format = vk::Format::eR8G8B8A8Srgb,
                    .colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear
                },
                vk::SurfaceFormatKHR {
                    .format = vk::Format::eB8G8R8A8Srgb,
                    .colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear
                }
            },
            .queueFamilyIndices = {},
            .usageFlags = vk::ImageUsageFlagBits::eColorAttachment
        };
    }

    struct FatSwapchainDescriptionExtended
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        vk::SharingMode sharingMode;
        vk::PresentModeKHR presentMode;
        vk::SurfaceFormatKHR format;
        vk::Extent2D extent;
        vk::SurfaceTransformFlagBitsKHR preTransform;
        vk::CompositeAlphaFlagBitsKHR compositeAlpha;
        u32 minImageCount;
    };

    Result<Unit, ErrorMessage> getSurfaceSupport(const FatSwapchainDescription& desc,
        const FatSwapchainDescriptionExtended& descExt)
    {
        auto supportResult = castVkResult(
            desc.physicalDevice->getSurfaceSupportKHR(desc.presentingQueueFamilyIndex, desc.surfaceDesc.surface));
        CHECK_KEEP(supportResult);
        bool supportsSurface = supportResult.take();
        
        if (!supportsSurface)
        {
            return ErrorMessage({ .channel = "vulkan" },
                "Surface present not supported by queue family {}.", desc.presentingQueueFamilyIndex);
        }

        return Ok();
    }
    
    Result<vk::PresentModeKHR, ErrorMessage> getPresentMode(const FatSwapchainDescription& desc,
        const FatSwapchainDescriptionExtended& descExt)
    {
        auto getPresentModesResult = castVkResult(desc.physicalDevice->getSurfacePresentModesKHR(desc.surfaceDesc.surface));
        CHECK_KEEP(getPresentModesResult);
        std::vector<vk::PresentModeKHR> availablePresentModes = getPresentModesResult.take();

        for (vk::PresentModeKHR presentMode : desc.presentModePriorities)
        {
            for (vk::PresentModeKHR availablePresentMode : availablePresentModes)
            {
                if (presentMode == availablePresentMode)
                {
                    return presentMode;
                }
            }
        }

        return ErrorMessage({ .channel = "vulkan" }, "Failed to find a suitable present mode.");
    }

    Result<vk::SurfaceFormatKHR, ErrorMessage> getFormat(const FatSwapchainDescription& desc,
        const FatSwapchainDescriptionExtended& descExt)
    {
        auto getFormatsResult = castVkResult(desc.physicalDevice->getSurfaceFormats2KHR({ .surface = desc.surfaceDesc.surface }));
        CHECK_KEEP(getFormatsResult);
        std::vector<vk::SurfaceFormat2KHR> availableFormats = getFormatsResult.take();
        
        for (vk::SurfaceFormatKHR format : desc.surfaceFormatPriorities)
        {
            for (vk::SurfaceFormat2KHR availableFormat : availableFormats)
            {
                if (format == availableFormat.surfaceFormat)
                {
                    return format;
                }
            }
        }

        warn({ .channel = "vulkan" }, "No available surface formats found in priorities; selected the first available.");

        return Result<vk::SurfaceFormatKHR, ErrorMessage>(availableFormats[0].surfaceFormat);
    }

    vk::Extent2D getExtent(const FatSwapchainDescription& desc,
        const FatSwapchainDescriptionExtended& descExt)
    {
        vk::Extent2D extent;

        bool swapchainDeterminesSurfaceSize = descExt.capabilities.currentExtent.width == 0xFFFFFFFF;
        if (swapchainDeterminesSurfaceSize)
        {
            extent = vk::Extent2D {
                .width = std::clamp<u32>(
                    desc.surfaceDesc.surfaceWidth,
                    descExt.capabilities.minImageExtent.width,
                    descExt.capabilities.maxImageExtent.width
                ),
                .height = std::clamp<u32>(
                    desc.surfaceDesc.surfaceHeight,
                    descExt.capabilities.minImageExtent.height,
                    descExt.capabilities.maxImageExtent.height
                )
            };
        }
        else
        {
            extent = descExt.capabilities.currentExtent;
        }

        return extent;
    }

    Result<vk::CompositeAlphaFlagBitsKHR, ErrorMessage> getCompositeAlpha(const FatSwapchainDescription& desc,
        const FatSwapchainDescriptionExtended& descExt)
    {
        if (!static_cast<bool>(descExt.capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque))
        {
            return ErrorMessage({ .channel = "vulkan" }, "Opaque composite alpha not supported by surface.");
        }

        return vk::CompositeAlphaFlagBitsKHR::eOpaque;
    }

    Result<u32, ErrorMessage> getMinImageCount(const FatSwapchainDescription& desc,
        const FatSwapchainDescriptionExtended& descExt)
    {
        u32 minImageCount;
        u32 desiredImageCount = descExt.capabilities.minImageCount + 1;
        
        bool noImageLimit = descExt.capabilities.maxImageCount == 0;
        if (noImageLimit)
        {
            minImageCount = desiredImageCount;
        }
        else
        {
            minImageCount = std::max<u32>(
                desiredImageCount,
                descExt.capabilities.maxImageCount
            );
        }
        return minImageCount;
    }

    Result<FatSwapchainDescriptionExtended, ErrorMessage> processDescription(FatSwapchainDescription& desc)
    {
        FatSwapchainDescriptionExtended descExt;

        // check for nullptrs
        if (desc.device == nullptr)
            return ErrorMessage({ .channel = "vulkan" }, "Device in swapchain creation cannot be nullptr.");
        if (desc.physicalDevice == nullptr)
            return ErrorMessage({ .channel = "vulkan" }, "Physical device in swapchain creation cannot be nullptr.");

        // get surface capabilities
        vk::PhysicalDeviceSurfaceInfo2KHR surfaceInfo { .surface = desc.surfaceDesc.surface };
        auto surfaceCapabilitiesResult
            = castVkResult(desc.physicalDevice->getSurfaceCapabilities2KHR(surfaceInfo));
        CHECK_KEEP(surfaceCapabilitiesResult);
        descExt.capabilities = surfaceCapabilitiesResult.take().surfaceCapabilities;
        
        // check that presenting queue family supports presentation
        CHECK(getSurfaceSupport(desc, descExt));

        // get sharing mode
        // TODO: `sharingMode` should probably be specified by the user instead
        vk::SharingMode sharingMode;
        if (desc.queueFamilyIndices.empty())
            descExt.sharingMode = vk::SharingMode::eConcurrent;
        else
            descExt.sharingMode = vk::SharingMode::eExclusive;
        
        // get present mode
        auto getPresentModeResult = getPresentMode(desc, descExt);
        CHECK_KEEP(getPresentModeResult);
        descExt.presentMode = getPresentModeResult.take();

        // get format
        auto getFormatResult = getFormat(desc, descExt);
        CHECK_KEEP(getFormatResult);
        descExt.format = getFormatResult.take();

        // get extent
        descExt.extent = getExtent(desc, descExt);

        // get pre-transform
        descExt.preTransform = descExt.capabilities.currentTransform;

        // get composite alpha
        auto getCompositeAlphaResult = getCompositeAlpha(desc, descExt);
        CHECK_KEEP(getCompositeAlphaResult);
        descExt.compositeAlpha = getCompositeAlphaResult.take();

        // get min image count
        auto getMinImageCountResult = getMinImageCount(desc, descExt);
        CHECK_KEEP(getMinImageCountResult);
        descExt.minImageCount = getMinImageCountResult.take();

        return descExt;
    }

    Result<Unit, ErrorMessage> FatSwapchain::retrieveImages()
    {
        auto getImagesResult = castVkResult(this->swapchain_.getImages());
        CHECK_KEEP(getImagesResult);
        this->images_ = getImagesResult.take();

        for (u32 i = 0; i < this->images_.size(); i++)
        {
            vk::ImageViewCreateInfo viewInfo {
                .image = this->images_[i],
                .viewType = vk::ImageViewType::e2D,
                .format = this->format_.format,
                .subresourceRange = vk::ImageSubresourceRange {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };
            auto viewResult = castVkResult(this->cachedDesc_.device->createImageView(viewInfo));
            CHECK_KEEP(viewResult);
            this->imageViews_.emplace_back(viewResult.take());
        }

        return Ok();
    }
    
    Result<Unit, ErrorMessage> FatSwapchain::initialize()
    {
        FatSwapchainDescription& desc = this->cachedDesc_;

        // extend the description
        auto processResult = processDescription(desc);
        CHECK_KEEP(processResult);
        FatSwapchainDescriptionExtended descExt = processResult.take();

        // create the swapchain
        vk::SwapchainCreateInfoKHR createInfo {
            .flags = {},
            .surface = desc.surfaceDesc.surface,
            .minImageCount = descExt.minImageCount,
            .imageFormat = descExt.format.format,
            .imageColorSpace = descExt.format.colorSpace,
            .imageExtent = descExt.extent,
            .imageArrayLayers = 1,
            .imageUsage = desc.usageFlags,
            .imageSharingMode = descExt.sharingMode,
            .queueFamilyIndexCount = static_cast<u32>(desc.queueFamilyIndices.size()),
            .pQueueFamilyIndices = desc.queueFamilyIndices.data(),
            .preTransform = descExt.preTransform,
            .compositeAlpha = descExt.compositeAlpha,
            .presentMode = descExt.presentMode,
            .clipped = true,
            .oldSwapchain = *this->swapchain_
        };

        auto swapchainCreateResult = castVkResult(desc.device->createSwapchainKHR(createInfo));
        CHECK_KEEP(swapchainCreateResult);
        vk::raii::SwapchainKHR swapchain = swapchainCreateResult.take();

        // initialize the fat swapchain
        this->swapchain_ = std::move(swapchain);
        this->cachedDesc_ = std::move(desc);
        this->extent_ = descExt.extent;
        this->format_ = descExt.format;

        // retrieve its images
        CHECK(this->retrieveImages());

        return Ok();
    }
    
    Result<FatSwapchain, ErrorMessage> FatSwapchain::create(FatSwapchainDescription desc)
    {        
        // initial-construction-specific checks
        if (desc.surfaceDesc.surface == VK_NULL_HANDLE)
            return ErrorMessage({ .channel = "vulkan" }, "Surface in swapchain creation cannot be nullptr.");

        FatSwapchain swapchain;
        swapchain.cachedDesc_ = std::move(desc);
        CHECK(swapchain.initialize());
        return std::move(swapchain);
    }
    
    Result<Unit, ErrorMessage> FatSwapchain::recreate(SurfaceDescription surfaceDesc)
    {
        // TODO: recreate the swapchain
        this->cachedDesc_.surfaceDesc.surfaceWidth = surfaceDesc.surfaceWidth;
        this->cachedDesc_.surfaceDesc.surfaceHeight = surfaceDesc.surfaceHeight;
        if (surfaceDesc.surface != VK_NULL_HANDLE)
            this->cachedDesc_.surfaceDesc.surface = surfaceDesc.surface;
        
        CHECK(this->initialize());

        // TODO: emit an event here

        return Ok();
    }
}