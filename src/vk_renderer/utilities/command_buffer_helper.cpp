#include "command_buffer_helper.h"
#include "../geometry/mesh.h"
#include "../geometry/texture.h"

namespace VkRenderer::Utilities
{
    void CommandBufferHelper::bindMesh(const Geometry::Mesh& mesh)
    {
        this->cmdBuffer_.bindVertexBuffers(0, **mesh.vertexBuffer, { 0 });
        this->cmdBuffer_.bindIndexBuffer(**mesh.indexBuffer, 0, vk::IndexType::eUint32);
    }

    void CommandBufferHelper::drawMesh(const Geometry::Mesh& mesh, u32 instanceCount)
    {
        this->cmdBuffer_.drawIndexed(mesh.data.numIndices, instanceCount, 0, 0, 0);
    }

    MemoryState CommandBufferHelper::addMemoryBarrier(MemoryState src, MemoryState dst)
    {
        vk::MemoryBarrier2 barrier {
            .srcStageMask = src.stageMask,
            .srcAccessMask = src.accessMask,
            .dstStageMask = dst.stageMask,
            .dstAccessMask = dst.accessMask
        };

        vk::DependencyInfo dependencyInfo {
            .memoryBarrierCount = 1,
            .pMemoryBarriers = &barrier
        };

        this->cmdBuffer_.pipelineBarrier2(dependencyInfo);

        return dst;
    }
    
    ImageState CommandBufferHelper::addImageBarrier(const Geometry::Texture& tex, ImageState src,
        ImageState dst, Option<vk::ImageSubresourceRange> subresourceRangeOverride)
    {
        vk::ImageSubresourceRange subresourceRange;
        if (subresourceRangeOverride)
        {
            subresourceRange = subresourceRangeOverride.value();
        }
        else
        {
            subresourceRange = vk::ImageSubresourceRange {
                .aspectMask = tex.info.aspectFlags,
                .baseMipLevel = 0,
                .levelCount = tex.info.mipLevels,
                .baseArrayLayer = 0,
                .layerCount = tex.info.numLayers
            };
        }

        vk::ImageMemoryBarrier2 barrier {
            .srcStageMask = src.stageMask,
            .srcAccessMask = src.accessMask,
            .dstStageMask = dst.stageMask,
            .dstAccessMask = dst.accessMask,
            .oldLayout = src.layout,
            .newLayout = dst.layout,
            .srcQueueFamilyIndex = src.queueFamilyIndex,
            .dstQueueFamilyIndex = dst.queueFamilyIndex,
            .image = *tex.image,
            .subresourceRange = std::move(subresourceRange)
        };

        vk::DependencyInfo dependencyInfo {
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrier
        };

        this->cmdBuffer_.pipelineBarrier2(dependencyInfo);
        
        return dst;
    }
}
