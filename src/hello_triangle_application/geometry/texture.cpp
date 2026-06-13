#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "../hello_triangle_application.h"
#include "texture.h"
#include "buffer.h"
#include "../utilities/command_buffer.h"
#include "vertex_buffer.h"
#include "../utilities/image_views.h"

using namespace HelloTriangleApplication::Utilities;

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> Texture::makeSampler(App& app)
    {
        vk::PhysicalDeviceProperties deviceProperties = app.physicalDevice.getProperties();

        vk::SamplerCreateInfo samplerInfo {
            .magFilter = vk::Filter::eLinear,
            .minFilter = vk::Filter::eLinear,
            .mipmapMode = vk::SamplerMipmapMode::eLinear,
            .addressModeU = vk::SamplerAddressMode::eRepeat,
            .addressModeV = vk::SamplerAddressMode::eRepeat,
            .addressModeW = vk::SamplerAddressMode::eRepeat,
            .mipLodBias = 0.0f,
            .anisotropyEnable = vk::True,
            .maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy,
            .compareEnable = vk::False,
            .compareOp = vk::CompareOp::eAlways,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = vk::BorderColor::eIntOpaqueBlack,
            .unnormalizedCoordinates = vk::False
        };

        auto samplerResult = castVkResult(app.device.createSampler(samplerInfo));
        if (!samplerResult)
            return Err<std::string>("Failed to create texture sampler.");
        
        this->sampler = std::make_shared<vk::raii::Sampler>(samplerResult.take());

        return Ok();
    }

    Result<Texture, std::string> Texture::load(App& app, std::string path, vk::Format format,
        vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
    {
        // load image into main memory
        i32 texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if (!pixels)
            return Err<std::string>("Failed to load texture " + path + ".");
        
        // create staging buffer
        vk::DeviceSize imageSize = texWidth * texHeight * 4;
        
        FatBuffer imageBuffer;
        TRY_ASSIGN(
            imageBuffer,
            FatBuffer::create(
                app,
                imageSize,
                vk::BufferUsageFlagBits::eTransferSrc,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );

        auto mapResult = castVkResult(imageBuffer.memory.mapMemory(0, imageSize));
        CHECK_KEEP(mapResult);
        void* data = mapResult.take();

        memcpy(data, pixels, imageSize);
        imageBuffer.memory.unmapMemory();

        // free copy of image in main memory
        stbi_image_free(pixels);

        return Texture::create(app, texWidth, texHeight, imageBuffer, format, tiling, usage, properties);
    }

    Result<Texture, std::string> Texture::create(App& app, u32 width, u32 height, FatBuffer& imageBuffer,
        vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
    {
        // create GPU-backed image
        vk::ImageCreateInfo imageInfo {
            .imageType = vk::ImageType::e2D,
            .format = format,
            .extent = { width, height, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = vk::SampleCountFlagBits::e1,
            .tiling = tiling,
            .usage = usage,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined
        };

        Texture texture {};
        TRY_ASSIGN(
            texture.image,
            castVkResult(app.device.createImage(imageInfo))
        );

        // allocate image memory
        vk::MemoryRequirements memRequirements = texture.image.getMemoryRequirements();
        vk::MemoryAllocateInfo allocInfo {
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryType(app, memRequirements.memoryTypeBits, properties)
        };

        TRY_ASSIGN(
            texture.memory,
            castVkResult(app.device.allocateMemory(allocInfo))
        );

        texture.image.bindMemory(texture.memory, 0);

        // copy buffer to image
        // TODO: use transfer queue
        vk::raii::CommandBuffer copyCommand = nullptr;
        TRY_ASSIGN(
            copyCommand,
            beginSingleTimeCommands(app, app.graphicsQueue)
        );

        transitionImageLayout(copyCommand, texture.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        copyBufferToImage(copyCommand, imageBuffer.buffer, texture.image, width, height);
        transitionImageLayout(copyCommand, texture.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
        
        endSingleTimeCommands(std::move(copyCommand), app.graphicsQueue);

        // create image view
        TRY_ASSIGN(
            texture.view,
            createImageView(app, texture.image, format)
        );

        return std::move(texture);
    }
}