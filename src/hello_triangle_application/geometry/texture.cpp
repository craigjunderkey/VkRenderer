#include "../hello_triangle_application.h"
#include "texture.h"
#include "buffer.h"
#include "../utilities/command_buffer.h"
#include "vertex_buffer.h"
#include "../utilities/image_views.h"
#include "../resources/image_data.h"

using namespace HelloTriangleApplication::Utilities;
using namespace HelloTriangleApplication::Resources;

namespace HelloTriangleApplication::Geometry
{
    vk::SamplerCreateInfo Texture::defaultSamplerInfo(App& app)
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
            .maxLod = vk::LodClampNone,
            .borderColor = vk::BorderColor::eIntOpaqueBlack,
            .unnormalizedCoordinates = vk::False
        };

        return samplerInfo;
    }

    Result<Unit, std::string> generateMipmaps(App& app, vk::raii::CommandBuffer& commandBuffer, vk::raii::Image& image, u32 texWidth, u32 texHeight, u32 mipLevels, u32 numLayers, vk::Format format)
    {
        // check whether the image format supports blitting
        vk::FormatProperties formatProperties = app.physicalDevice.getFormatProperties(format);
        bool supportsLinearFiltering = static_cast<bool>(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear);
        if (!supportsLinearFiltering)
        {   
            return Err<std::string>("Image format " + vk::to_string(format) + " does not support linear filtering, so mipmaps cannot be generated.");
        }
        
        // prepare image barrier with shared properties
        vk::ImageMemoryBarrier barrier {
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = *image,
            .subresourceRange {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .levelCount = 1,
                .layerCount = numLayers,
            }
        };

        u32 mipWidth = texWidth;
        u32 mipHeight = texHeight;
        for (u32 i = 1; i < mipLevels; i++)
        {
            // prepare image for copying previous mip
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;

            commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barrier);

            // blit previous mip to current mip
            u32 nextMipWidth = std::max(1u, mipWidth / 2);
            u32 nextMipHeight = std::max(1u, mipHeight / 2);
            vk::ImageBlit blit {
                .srcSubresource = vk::ImageSubresourceLayers {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = i - 1,
                    .layerCount = numLayers
                },
                .srcOffsets = std::array<vk::Offset3D, 2>({
                    {},
                    { (i32)mipWidth, (i32)mipHeight, 1 }
                }),
                .dstSubresource = vk::ImageSubresourceLayers {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = i,
                    .layerCount = numLayers
                },
                .dstOffsets = std::array<vk::Offset3D, 2>({
                    {},
                    { (i32)nextMipWidth, (i32)nextMipHeight, 1 }
                })
            };

            commandBuffer.blitImage(*image, vk::ImageLayout::eTransferSrcOptimal,
                *image, vk::ImageLayout::eTransferDstOptimal,
                blit, vk::Filter::eLinear);
            
            // prepare previous mip for being read by shaders
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

            commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);

            // update current mip dimensions
            mipWidth = nextMipWidth;
            mipHeight = nextMipHeight;
        }

        // prepare final mip for being read by shaders, since we don't blit from it in the loop
        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);
        
        return Ok();
    }

    Result<Texture, std::string> Texture::loadFromImageData(App& app, const ImageData& imageData, vk::Format format,
        vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::ImageAspectFlags aspectFlags,
        bool doMipmapping, vk::SampleCountFlagBits sampleCount)
    {
        // create staging buffer
        vk::DeviceSize imageSize = imageData.width * imageData.height * 4;
        
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

        // transfer image data to staging buffer
        memcpy(data, imageData.pixels, imageSize);
        imageBuffer.memory.unmapMemory();

        return Texture::loadFromBuffer(app, imageData.width, imageData.height, imageBuffer, format, tiling,
            usage, properties, aspectFlags, doMipmapping, sampleCount);
    }

    Result<Texture, std::string> Texture::loadFromBuffer(App& app, u32 width, u32 height, FatBuffer& imageBuffer,
        vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::ImageAspectFlags aspectFlags,
        bool doMipmapping, vk::SampleCountFlagBits sampleCount)
    {
        // create GPU-backed image
        Texture texture;
        TRY_ASSIGN(
            texture,
            Texture::create(app, width, height, format, tiling, usage, properties, aspectFlags, doMipmapping, sampleCount)
        );

        // copy buffer to image
        // TODO: use transfer queue
        vk::raii::CommandBuffer copyCommand = nullptr;
        TRY_ASSIGN(
            copyCommand,
            beginSingleTimeCommands(app, app.graphicsQueue)
        );

        CHECK(transitionImageLayout(copyCommand, texture.image, vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal, texture.mipLevels));
        
        copyBufferToImage(copyCommand, imageBuffer.buffer, texture.image, width, height);
        
        if (doMipmapping)
        {
            CHECK(generateMipmaps(app, copyCommand, texture.image, width, height, texture.mipLevels, 1, format));
        }
        else
        {
            CHECK(transitionImageLayout(copyCommand, texture.image, vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal, 1));
        }

        endSingleTimeCommands(std::move(copyCommand), app.graphicsQueue);

        return std::move(texture);
    }

    Result<Texture, std::string> Texture::create(App& app, u32 width, u32 height, vk::Format format,
            vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::ImageAspectFlags aspectFlags,
            bool doMipmapping, vk::SampleCountFlagBits sampleCount)
    {
        // compute miplevels
        u32 mipLevels = 1;
        if (doMipmapping)
            mipLevels = Texture::computeMipLevels(width, height);
        
        // create texture object
        Texture texture { .format = format, .width = width, .height = height, .mipLevels = mipLevels };    
        
        // create GPU-backed image
        vk::ImageCreateInfo imageInfo {
            .imageType = vk::ImageType::e2D,
            .format = format,
            .extent = { width, height, 1 },
            .mipLevels = mipLevels,
            .arrayLayers = 1,
            .samples = sampleCount,
            .tiling = tiling,
            .usage = usage,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined
        };

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

        // create image view
        TRY_ASSIGN(
            texture.view,
            createImageView(app, texture.image, format, aspectFlags, texture.mipLevels)
        );

        return std::move(texture);
    }

    Result<Texture, std::string> Texture::loadFromDescription(App& app, Resources::TextureDescription desc)
    {
        const ImageData& imageData = app.resources.get<ImageData>(desc.image);

        // create staging buffer
        auto bufferResult = FatBuffer::create(
            app, imageData.size(), vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
        );
        CHECK_KEEP(bufferResult);
        FatBuffer stagingBuffer = bufferResult.take();

        // transfer image data to staging buffer
        auto memoryMapResult = castVkResult(stagingBuffer.memory.mapMemory(0, imageData.size()));
        CHECK_KEEP(memoryMapResult);
        void* bufferData = memoryMapResult.take();
        memcpy(bufferData, imageData.pixels, imageData.size());
        stagingBuffer.memory.unmapMemory();

        // get mip levels and samples
        u32 mipLevels = 1;
        if (desc.doMipmapping)
            mipLevels = Texture::computeMipLevels(imageData.width, imageData.height);

        vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1;
        if (desc.doMultisampling)
            numSamples = app.msaaSamples;
        
        // create texture object
        Texture texture {
            .format = desc.format,
            .width = static_cast<u32>(imageData.width),
            .height = static_cast<u32>(imageData.height),
            .mipLevels = mipLevels,
            .numLayers = desc.numLayers
        };
        
        // create GPU image
        vk::ImageCreateInfo imageCreateInfo {
            .imageType = desc.imageType,
            .format = desc.format,
            .extent = { texture.width, texture.height, 1 },
            .mipLevels = mipLevels,
            .arrayLayers = desc.numLayers,
            .samples = numSamples,
            .tiling = desc.tiling,
            .usage = desc.usage,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined
        };

        auto imageCreateResult = castVkResult(app.device.createImage(imageCreateInfo));
        CHECK_KEEP(imageCreateResult);
        texture.image = imageCreateResult.take();

        // allocate and bind image memory
        vk::MemoryRequirements memRequirements = texture.image.getMemoryRequirements();
        vk::MemoryAllocateInfo allocInfo {
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryType(app, memRequirements.memoryTypeBits, desc.properties)
        };
        auto allocResult = castVkResult(app.device.allocateMemory(allocInfo));
        CHECK_KEEP(allocResult);
        texture.memory = allocResult.take();
        texture.image.bindMemory(texture.memory, 0);

        // create image view
        vk::ImageViewCreateInfo viewCreateInfo {
            .image = *texture.image,
            .viewType = desc.viewType,
            .format = desc.format,
            .subresourceRange = {
                .aspectMask = desc.aspectFlags,
                .baseMipLevel = 0,
                .levelCount = mipLevels,
                .baseArrayLayer = 0,
                .layerCount = desc.numLayers
            }
        };
        auto viewCreateResult = castVkResult(app.device.createImageView(viewCreateInfo));
        CHECK_KEEP(viewCreateResult);
        texture.view = viewCreateResult.take();

        // prepare command buffer for copying
        auto commandBeginResult = beginSingleTimeCommands(app, app.graphicsQueue);
        CHECK_KEEP(commandBeginResult);
        vk::raii::CommandBuffer copyCommand = commandBeginResult.take();

        CHECK(transitionImageLayout(
            copyCommand, *texture.image, vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal, mipLevels, desc.numLayers, desc.aspectFlags
        ));

        // copy buffer to image
        vk::BufferImageCopy copyRegion {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = desc.aspectFlags,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = texture.numLayers
            },
            .imageOffset = { 0, 0, 0 },
            .imageExtent = { texture.width, texture.height, 1 }
        };
        copyCommand.copyBufferToImage(**stagingBuffer, *texture.image, vk::ImageLayout::eTransferDstOptimal, copyRegion);

        // do mipmapping or transfer image to shader-read optimal
        if (desc.doMipmapping)
        {
            CHECK(generateMipmaps(
                app, copyCommand, texture.image, texture.width, texture.height, texture.mipLevels, texture.numLayers, texture.format
            ));
        }
        else
        {
            CHECK(transitionImageLayout(
                copyCommand, *texture.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
                texture.mipLevels, texture.numLayers, desc.aspectFlags
            ));
        }

        endSingleTimeCommands(std::move(copyCommand), app.graphicsQueue);

        return std::move(texture);
    }

    u32 Texture::computeMipLevels(u32 texWidth, u32 texHeight)
    {
        return static_cast<u32>(std::floor(std::log2(std::max(texWidth, texHeight))) + 1);
    }

    bool Texture::isMipmapped() const
    {
        return this->mipLevels == 1;
    }
}