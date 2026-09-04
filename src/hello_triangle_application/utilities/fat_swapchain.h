#pragma once
#include "vulkan_common.h"

namespace HelloTriangleApplication::Utilities
{
    struct SurfaceDescription
    {
        // `surface` can be VK_NULL_HANDLE iff recreating the swapchain
        vk::SurfaceKHR surface;

        u32 surfaceWidth;
        u32 surfaceHeight;
    };

    struct FatSwapchainDescription
    {
        // if `device` is nullptr, an error is returned
        const vk::raii::Device* device;

        // if `physicalDevice` is nullptr, an error is returned
        const vk::raii::PhysicalDevice* physicalDevice;
        
        SurfaceDescription surfaceDesc;

        // present modes are queried from first to last
        // if no item in `presentModePriorities` is supported, an error is returned
        std::vector<vk::PresentModeKHR> presentModePriorities;

        // surface formats are queried from first to last
        // if no item in `surfaceFormatPriorities` is supported, the first available is selected and a warning is outputted
        std::vector<vk::SurfaceFormatKHR> surfaceFormatPriorities;

        // if non-empty, sharing mode is set to exclusive
        std::vector<u32> queueFamilyIndices;

        // index of the queue family with which the swapchain will be presented
        // if `presentingQueueFamilyIndex` isn't suppoted, an error is returned
        u32 presentingQueueFamilyIndex;

        // if `usageFlags` isn't a subset of the surface's supported usage flags, an error is returned
        vk::ImageUsageFlags usageFlags;

        /* leaves only the following fields undefined:
            - `device`
            - `physicalDevice`
            - `surfaceDesc`
            - `presentingQueueFamilyIndex`
        */
        static FatSwapchainDescription makeDefault();
    };
    
    class FatSwapchain
    {
    private:
        vk::raii::SwapchainKHR swapchain_ = nullptr;
        std::vector<vk::Image> images_ = {};
        std::vector<vk::raii::ImageView> imageViews_ = {};
        FatSwapchainDescription cachedDesc_ = {};

        vk::Extent2D extent_;
        vk::SurfaceFormatKHR format_;

        Result<Unit, ErrorMessage> retrieveImages();
        Result<Unit, ErrorMessage> initialize();

    public:
        static Result<FatSwapchain, ErrorMessage> create(FatSwapchainDescription desc);

        Result<Unit, ErrorMessage> recreate(SurfaceDescription surfaceDesc);

        constexpr const std::vector<vk::Image>& getImages() const { return this->images_; }
        constexpr const std::vector<vk::raii::ImageView>& getImageViews() const { return this->imageViews_; }
        constexpr const std::vector<u32>& getQueueFamilies() const { return this->cachedDesc_.queueFamilyIndices; }
        constexpr vk::Extent2D getExtent() const { return this->extent_; }
        constexpr vk::SurfaceFormatKHR getFormat() const { return this->format_; }

        constexpr vk::raii::SwapchainKHR& getSwapchain()
        {
            assert(this->swapchain_ != nullptr);
            return this->swapchain_;
        }
        constexpr const vk::raii::SwapchainKHR& getSwapchain() const
        {
            assert(this->swapchain_ != nullptr);
            return this->swapchain_;
        }
        constexpr vk::raii::SwapchainKHR& operator*() { return this->getSwapchain(); }
        constexpr const vk::raii::SwapchainKHR& operator*() const { return this->getSwapchain(); }
        constexpr vk::raii::SwapchainKHR* operator->() { return &this->getSwapchain(); }
    };
}