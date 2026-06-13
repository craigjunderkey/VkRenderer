#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "buffer.h"
#include <memory>

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    struct Texture
    {
        vk::raii::Image image = nullptr;
        vk::raii::ImageView view = nullptr;
        vk::raii::DeviceMemory memory = nullptr;
        std::shared_ptr<vk::raii::Sampler> sampler = nullptr;

        static Result<Texture, std::string> load(App& app, std::string path,
            vk::Format format = vk::Format::eR8G8B8A8Srgb,
            vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
            vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal
        );

        static Result<Texture, std::string> create(App& app, u32 width, u32 height, FatBuffer& imageBuffer,
            vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
        
        Result<Unit, std::string> makeSampler(App& app);
    };
}