#include "../vk_renderer.h"
#include "texture.h"
#include "buffer.h"
#include "../utilities/command_buffer.h"
#include "vertex_buffer.h"
#include "../utilities/image_views.h"
#include "../resources/image_data.h"
#include "../utilities/command_buffer_helper.h"

using namespace VkRenderer::Utilities;
using namespace VkRenderer::Resources;

namespace VkRenderer::Geometry
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

    Result<ImageState, ErrorMessage> generateMipmaps(App& app, vk::raii::CommandBuffer& commandBuffer, Texture& tex, ImageState srcState)
    {
        assert(srcState.layout == vk::ImageLayout::eTransferDstOptimal);
        assert(srcState.stageMask == vk::PipelineStageFlagBits2::eTransfer);
        assert(srcState.accessMask == vk::AccessFlagBits2::eTransferWrite);
        
        // check whether the image format supports blitting
        vk::FormatProperties formatProperties = app.physicalDevice.getFormatProperties(tex.info.format);
        bool supportsLinearFiltering = static_cast<bool>(
            formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear
        );
        if (!supportsLinearFiltering)
        {   
            return ErrorMessage(
                { .channel = "vulkan" },
                "Image format {} does not support linear filtering, so mipmaps cannot be generated.",
                vk::to_string(tex.info.format)
            );
        }
        
        // prepare image state
        CommandBufferHelper cmdBuffer = CommandBufferHelper(app, commandBuffer);
        ImageState regionState = srcState;
        vk::ImageSubresourceRange subresourceRange {
            .aspectMask = tex.info.aspectFlags,
            .levelCount = 1,
            .layerCount = tex.info.numLayers
        };
        
        u32 mipWidth = tex.info.width;
        u32 mipHeight = tex.info.height;
        for (u32 i = 1; i < tex.info.mipLevels; i++)
        {
            // prepare image for copying previous mip
            subresourceRange.baseMipLevel = i - 1;
            regionState = cmdBuffer.addImageBarrier(
                tex,
                regionState,
                ImageState {
                    .layout = vk::ImageLayout::eTransferSrcOptimal,
                    .queueFamilyIndex = regionState.queueFamilyIndex,
                    .accessMask = vk::AccessFlagBits2::eTransferRead,
                    .stageMask = vk::PipelineStageFlagBits2::eTransfer
                },
                subresourceRange
            );

            // blit previous mip to current mip
            u32 nextMipWidth = std::max(1u, mipWidth / 2);
            u32 nextMipHeight = std::max(1u, mipHeight / 2);
            vk::ImageBlit blit {
                .srcSubresource = vk::ImageSubresourceLayers {
                    .aspectMask = tex.info.aspectFlags,
                    .mipLevel = i - 1,
                    .layerCount = tex.info.numLayers
                },
                .srcOffsets = std::array<vk::Offset3D, 2>({
                    {},
                    { (i32)mipWidth, (i32)mipHeight, 1 }
                }),
                .dstSubresource = vk::ImageSubresourceLayers {
                    .aspectMask = tex.info.aspectFlags,
                    .mipLevel = i,
                    .layerCount = tex.info.numLayers
                },
                .dstOffsets = std::array<vk::Offset3D, 2>({
                    {},
                    { (i32)nextMipWidth, (i32)nextMipHeight, 1 }
                })
            };
            cmdBuffer->blitImage(*tex.image, regionState.layout,
                *tex.image, vk::ImageLayout::eTransferDstOptimal,
                blit, vk::Filter::eLinear);
            
            // prepare previous mip for being read by shaders
            subresourceRange.baseMipLevel = i - 1;
            regionState = cmdBuffer.addImageBarrier(
                tex,
                regionState,
                ImageState {
                    .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
                    .queueFamilyIndex = regionState.queueFamilyIndex,
                    .accessMask = vk::AccessFlagBits2::eShaderRead,
                    .stageMask = vk::PipelineStageFlagBits2::eFragmentShader
                },
                subresourceRange
            );

            // update current mip dimensions
            mipWidth = nextMipWidth;
            mipHeight = nextMipHeight;

            // reset region state
            regionState = srcState;
        }

        // prepare final mip for being read by shaders, since we don't blit from it in the loop
        subresourceRange.baseMipLevel = tex.info.mipLevels - 1;
        regionState = cmdBuffer.addImageBarrier(
            tex,
            regionState,
            ImageState {
                .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
                .queueFamilyIndex = regionState.queueFamilyIndex,
                .accessMask = vk::AccessFlagBits2::eShaderRead,
                .stageMask = vk::PipelineStageFlagBits2::eFragmentShader
            },
            subresourceRange
        );
        
        return std::move(regionState);
    }

    Result<FatBuffer, ErrorMessage> makeImageCopyBuffer(App& app, const TextureDescription& desc)
    {
        u32 numImages = 1 + desc.additionalImages.size();
        vk::DeviceSize imageSize = app.resources.get<ImageData>(desc.image).size();

        // create staging buffer
        auto bufferResult = FatBuffer::create(
            app, imageSize * numImages, vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
        );
        CHECK_KEEP(bufferResult);
        FatBuffer stagingBuffer = bufferResult.take();

        // transfer image data to staging buffer
        auto memoryMapResult = stagingBuffer.memory->map(0, imageSize * numImages);
        CHECK_KEEP(memoryMapResult);
        Utilities::HostMemoryMapping bufferData = memoryMapResult.take();

        for (u32 i = 0; i < numImages; i++)
        {
            // get image
            StringId image;
            if (i == 0)
                image = desc.image;
            else
                image = desc.additionalImages[i - 1];
            
            // copy image data
            const ImageData& imageData = app.resources.get<ImageData>(image);
            memcpy(static_cast<u8*>(*bufferData) + i * imageSize, imageData.pixels, imageSize);
        }
        
        return std::move(stagingBuffer);
    }

    ImageState addBufferToImageCopy(App& app, vk::raii::CommandBuffer& commandBuffer,
        FatBuffer& stagingBuffer, Texture& tex, ImageState srcState)
    {
        // prepare command buffer for copying
        CommandBufferHelper cmdBuffer = CommandBufferHelper(app, commandBuffer);
        ImageState dstState = cmdBuffer.addImageBarrier(
            tex,
            srcState,
            ImageState {
                .layout = vk::ImageLayout::eTransferDstOptimal,
                .queueFamilyIndex = srcState.queueFamilyIndex,
                .accessMask = vk::AccessFlagBits2::eTransferWrite,
                .stageMask = vk::PipelineStageFlagBits2::eTransfer
            }
        );

        // copy buffer to image
        vk::BufferImageCopy copyRegion {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = tex.info.aspectFlags,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = tex.info.numLayers
            },
            .imageOffset = { 0, 0, 0 },
            .imageExtent = { tex.info.width, tex.info.height, tex.info.depth }
        };

        cmdBuffer->copyBufferToImage(**stagingBuffer, *tex.image, dstState.layout, copyRegion);

        return dstState;
    }

    Result<Texture, ErrorMessage> Texture::fromDescription(App& app, const Resources::TextureDescription& desc)
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
            .info = TextureInfo {
                .format = desc.format,
                .aspectFlags = desc.aspectFlags,
                .width = layerDims.width,
                .height = layerDims.height,
                .depth = layerDims.depth,
                .mipLevels = mipLevels,
                .numLayers = desc.numLayers
            }
        };
        
        // create GPU image
        vk::ImageCreateInfo imageCreateInfo {
            .flags = desc.createFlags,
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
        auto allocResult = app.memoryHelper.allocate(
            memRequirements.size,
            desc.memoryProperties,
            memRequirements
        );
        CHECK_KEEP(allocResult);
        texture.memory = allocResult.take();
        CHECK(castVkResult(texture.image.bindMemory(**texture.memory, 0)));

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
        auto viewCreateResult = app.vkResFactory.get<ImageViewFactory>().create(viewCreateInfo);
        CHECK_KEEP(viewCreateResult);
        texture.view = viewCreateResult.take();

        // create a command buffer
        auto commandBeginResult = beginSingleTimeCommands(app, app.graphicsQueue);
        CHECK_KEEP(commandBeginResult);
        CommandBufferHelper cmdBuffer = CommandBufferHelper(app, commandBeginResult.value());

        // copy image to texture, if there is one
        ImageState imageState = ImageState {
            .layout = vk::ImageLayout::eUndefined,
            .queueFamilyIndex = app.graphicsQueue.familyIndex,
            .accessMask = {}, // freshly allocated, so nothing to flush
            .stageMask = vk::PipelineStageFlagBits2::eBottomOfPipe
        };
        Option<FatBuffer> copyBuffer;

        if (desc.image)
        {
            auto bufferResult = makeImageCopyBuffer(app, desc);
            CHECK_KEEP(bufferResult);
            copyBuffer = bufferResult.take();
            imageState = addBufferToImageCopy(app, *cmdBuffer, copyBuffer.value(), texture, imageState);
        }
        
        // do mipmapping or transfer image to shader-read optimal
        if (desc.doMipmapping)
        {
            if (imageState.layout != vk::ImageLayout::eTransferDstOptimal)
            {
                imageState = cmdBuffer.addImageBarrier(
                    texture,
                    imageState,
                    ImageState {
                        .layout = vk::ImageLayout::eTransferDstOptimal,
                        .queueFamilyIndex = imageState.queueFamilyIndex,
                        .accessMask = vk::AccessFlagBits2::eTransferRead | vk::AccessFlagBits2::eTransferWrite,
                        .stageMask = vk::PipelineStageFlagBits2::eTransfer
                    },
                    {}
                );
            }

            auto mipMapResult = generateMipmaps(app, *cmdBuffer, texture, imageState);
            CHECK_KEEP(mipMapResult);
            imageState = mipMapResult.take();

        }
        else if (imageState.layout == vk::ImageLayout::eTransferDstOptimal)
        {
            imageState = cmdBuffer.addImageBarrier(
                texture,
                imageState,
                ImageState {
                    .layout = vk::ImageLayout::eShaderReadOnlyOptimal,
                    .queueFamilyIndex = imageState.queueFamilyIndex,
                    .accessMask = vk::AccessFlagBits2::eShaderRead,
                    .stageMask = vk::PipelineStageFlagBits2::eFragmentShader
                }
            );
        }

        endSingleTimeCommands(commandBeginResult.take(), app.graphicsQueue);

        return std::move(texture);
    }

    u32 Texture::computeMipLevels(u32 texWidth, u32 texHeight)
    {
        return static_cast<u32>(std::floor(std::log2(std::max(texWidth, texHeight))) + 1);
    }

    bool Texture::isMipmapped() const
    {
        return this->info.mipLevels == 1;
    }
}