#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"

namespace HelloTriangleApplication::Resources
{
    struct TextureDescription
    {
        StringId image = StringId::NONE;
        vk::ImageType imageType;
        vk::ImageViewType viewType;
        bool doMipmapping;
        bool doMultisampling;
        vk::Format format;
        vk::ImageTiling tiling;
        vk::ImageUsageFlags usage;
        vk::MemoryPropertyFlags properties;
        vk::ImageAspectFlags aspectFlags;
        u32 numLayers;

        static TextureDescription defaultTexture()
        {
            return TextureDescription
            {
                .imageType = vk::ImageType::e2D,
                .viewType = vk::ImageViewType::e2D,
                .doMipmapping = true,
                .doMultisampling = false,
                .format = vk::Format::eR8G8B8A8Srgb,
                .tiling = vk::ImageTiling::eOptimal,
                .usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                .aspectFlags = vk::ImageAspectFlagBits::eColor,
                .numLayers = 1
            };
        }
    };
}