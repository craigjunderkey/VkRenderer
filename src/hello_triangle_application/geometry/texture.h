#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "buffer.h"
#include "../render_loop/sampler_manager.h"

#include <memory>

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    struct Texture
    {
        StringId name = StringId();
        vk::raii::Image image = nullptr;
        vk::raii::ImageView view = nullptr;
        vk::raii::DeviceMemory memory = nullptr;
        vk::Format format;
        u32 width = 0;
        u32 height = 0;
        u32 mipLevels = 1;

        static Result<Texture, std::string> loadFromFile(App& app, std::string path,
            vk::Format format = vk::Format::eR8G8B8A8Srgb,
            vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
            vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal,
            vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor,
            bool doMipmapping = true, vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1
        );

        static Result<Texture, std::string> loadFromBuffer(App& app, u32 width, u32 height, FatBuffer& imageBuffer,
            vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
            vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor,
            bool doMipmapping = true, vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1
        );
        
        static Result<Texture, std::string> create(App& app, u32 width, u32 height, vk::Format format,
            vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
            vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor,
            bool doMipmapping = true, vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1
        );
        
        static vk::SamplerCreateInfo defaultSamplerInfo(App& app);

        static u32 computeMipLevels(u32 texWidth, u32 texHeight);

        bool isMipmapped() const;
    };
}