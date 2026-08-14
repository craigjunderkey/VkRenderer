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

    Result<Unit, std::string> generateMipmaps(App& app, vk::raii::CommandBuffer& commandBuffer, Texture& tex)
    {
        // check whether the image format supports blitting
        vk::FormatProperties formatProperties = app.physicalDevice.getFormatProperties(tex.format);
        bool supportsLinearFiltering = static_cast<bool>(
            formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear
        );
        if (!supportsLinearFiltering)
        {   
            return Err<std::string>("Image format " + vk::to_string(tex.format)
                + " does not support linear filtering, so mipmaps cannot be generated.");
        }
        
        // prepare image barrier with shared properties
        vk::ImageMemoryBarrier barrier {
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = *tex.image,
            .subresourceRange {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .levelCount = 1,
                .layerCount = tex.numLayers,
            }
        };

        u32 mipWidth = tex.width;
        u32 mipHeight = tex.height;
        for (u32 i = 1; i < tex.mipLevels; i++)
        {
            // prepare image for copying previous mip
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barrier
            );

            // blit previous mip to current mip
            u32 nextMipWidth = std::max(1u, mipWidth / 2);
            u32 nextMipHeight = std::max(1u, mipHeight / 2);
            vk::ImageBlit blit {
                .srcSubresource = vk::ImageSubresourceLayers {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = i - 1,
                    .layerCount = tex.numLayers
                },
                .srcOffsets = std::array<vk::Offset3D, 2>({
                    {},
                    { (i32)mipWidth, (i32)mipHeight, 1 }
                }),
                .dstSubresource = vk::ImageSubresourceLayers {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = i,
                    .layerCount = tex.numLayers
                },
                .dstOffsets = std::array<vk::Offset3D, 2>({
                    {},
                    { (i32)nextMipWidth, (i32)nextMipHeight, 1 }
                })
            };
            commandBuffer.blitImage(*tex.image, vk::ImageLayout::eTransferSrcOptimal,
                *tex.image, vk::ImageLayout::eTransferDstOptimal,
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
        barrier.subresourceRange.baseMipLevel = tex.mipLevels - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);
        
        return Ok();
    }

    Result<FatBuffer, std::string> makeBufferForImageCopy(App& app, const ImageData& image)
    {
        // create staging buffer
        auto bufferResult = FatBuffer::create(
            app, image.size(), vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
        );
        CHECK_KEEP(bufferResult);
        FatBuffer stagingBuffer = bufferResult.take();

        // transfer image data to staging buffer
        auto memoryMapResult = castVkResult(stagingBuffer.memory.mapMemory(0, image.size()));
        CHECK_KEEP(memoryMapResult);
        void* bufferData = memoryMapResult.take();
        memcpy(bufferData, image.pixels, image.size());
        stagingBuffer.memory.unmapMemory();

        return std::move(stagingBuffer);
    }

    Result<Unit, std::string> addBufferToImageCopyCommand(App& app, vk::raii::CommandBuffer& cmdBuffer, FatBuffer& stagingBuffer,
        const TextureDescription& desc, Texture& tex)
    {
        // prepare command buffer for copying
        CHECK(transitionImageLayout(
            cmdBuffer, tex.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, tex.mipLevels, desc.numLayers, desc.aspectFlags
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
                .layerCount = tex.numLayers
            },
            .imageOffset = { 0, 0, 0 },
            .imageExtent = { tex.width, tex.height, tex.depth }
        };

        cmdBuffer.copyBufferToImage(**stagingBuffer, *tex.image, vk::ImageLayout::eTransferDstOptimal, copyRegion);

        return Ok();
    }

    Result<Texture, std::string> Texture::fromDescription(App& app, const Resources::TextureDescription& desc)
    {
        CHECK(desc.check(app.resources.registry().getMap<ImageData>()));

        // get layer dimensions
        vk::Extent3D layerDims;

        if (desc.image)
        {
            const ImageData& imageData = app.resources.get<ImageData>(desc.image);
            layerDims.width = static_cast<u32>(imageData.width);
            layerDims.height = static_cast<u32>(imageData.height);
            layerDims.depth = 1;
        }
        else
        {
            layerDims = desc.dimensions.value();
        }
        
        // get mip levels and samples
        u32 mipLevels = 1;
        if (desc.doMipmapping)
            mipLevels = Texture::computeMipLevels(layerDims.width, layerDims.height);

        vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1;
        if (desc.doMultisampling)
            numSamples = app.msaaSamples;
        
        // create texture object
        Texture texture {
            .format = desc.format,
            .width = layerDims.width,
            .height = layerDims.height,
            .depth = layerDims.depth,
            .mipLevels = mipLevels,
            .numLayers = desc.numLayers
        };
        
        // create GPU image
        vk::ImageCreateInfo imageCreateInfo {
            .imageType = desc.imageType,
            .format = desc.format,
            .extent = layerDims,
            .mipLevels = mipLevels,
            .arrayLayers = desc.numLayers,
            .samples = numSamples,
            .tiling = desc.tiling,
            .usage = desc.usageFlags,
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
            .memoryTypeIndex = findMemoryType(app, memRequirements.memoryTypeBits, desc.memoryProperties)
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

        // create a command buffer
        auto commandBeginResult = beginSingleTimeCommands(app, app.graphicsQueue);
        CHECK_KEEP(commandBeginResult);
        vk::raii::CommandBuffer cmdBuffer = commandBeginResult.take();

        // copy image to texture, if there is one
        vk::ImageLayout layout;
        Option<FatBuffer> copyBuffer;
        if (desc.image)
        {
            auto bufferResult = makeBufferForImageCopy(app, app.resources.get<ImageData>(desc.image));
            CHECK_KEEP(bufferResult);
            copyBuffer = bufferResult.take();
            addBufferToImageCopyCommand(app, cmdBuffer, copyBuffer.value(), desc, texture);
            layout = vk::ImageLayout::eTransferDstOptimal;
        }
        else
        {
            layout = vk::ImageLayout::eUndefined;
        }
        
        // do mipmapping or transfer image to shader-read optimal
        if (desc.doMipmapping)
        {
            if (layout != vk::ImageLayout::eTransferDstOptimal)
            {
                CHECK(transitionImageLayout(
                    cmdBuffer, *texture.image, layout, vk::ImageLayout::eTransferDstOptimal,
                    texture.mipLevels, texture.numLayers, desc.aspectFlags
                ));
                layout = vk::ImageLayout::eTransferDstOptimal;
            }
            CHECK(generateMipmaps(app, cmdBuffer, texture));
        }
        else if (layout == vk::ImageLayout::eTransferDstOptimal)
        {
            CHECK(transitionImageLayout(
                cmdBuffer, *texture.image, layout, vk::ImageLayout::eShaderReadOnlyOptimal,
                texture.mipLevels, texture.numLayers, desc.aspectFlags
            ));
        }

        endSingleTimeCommands(std::move(cmdBuffer), app.graphicsQueue);

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