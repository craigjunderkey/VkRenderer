#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "buffer.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    struct Texture
    {
        vk::raii::Image image = nullptr;
        vk::raii::DeviceMemory memory = nullptr;

        static Result<Texture, std::string> load(App& app, std::string path,
            vk::Format format = vk::Format::eR8G8B8A8Srgb,
            vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
            vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal
        );

        static Result<Texture, std::string> create(App& app, u32 width, u32 height, FatBuffer& imageBuffer,
            vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
    };
}