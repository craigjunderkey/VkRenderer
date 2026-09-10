#pragma once
#include "vulkan_common.h"
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<vk::raii::ImageView, ErrorMessage> createImageView(App& app, const vk::Image& image,
        vk::Format format = vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor,
        u32 mipLevels = 1);
}