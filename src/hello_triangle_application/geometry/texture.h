#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "buffer.h"
#include "../resources/texture_description.h"

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
        u32 depth = 1;
        u32 mipLevels = 1;
        u32 numLayers = 1;

        static Result<Texture, std::string> fromDescription(App& app, const Resources::TextureDescription& desc);
        
        static vk::SamplerCreateInfo defaultSamplerInfo(App& app);

        static u32 computeMipLevels(u32 texWidth, u32 texHeight);

        bool isMipmapped() const;
    };
}