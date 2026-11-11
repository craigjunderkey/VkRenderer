#include "../vk_renderer.h"
#include "vertex_buffer.h"
#include "vertex.h"
#include "buffer.h"
#include "../utilities/command_buffer.h"
#include "../utilities/vulkan_debug.h"

using VkRenderer::App;
using VkRenderer::Geometry::Vertex;
using VkRenderer::Geometry::FatBuffer;

using namespace VkRenderer::Utilities;

namespace VkRenderer::Geometry
{
    Result<FatBuffer, ErrorMessage> createStagingBuffer(App& app, void* sourceData, vk::DeviceSize size)
    {
        std::vector<u32> families = {
            app.graphicsQueue.familyIndex,
            app.transferQueue.familyIndex
        };

        // create the staging buffer
        auto createResult = FatBuffer::create(
            app, size, vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            std::move(families)
        );
        CHECK_KEEP(createResult);
        FatBuffer stagingBuffer = createResult.take();

        CHECK(setDebugName(app, **stagingBuffer, "Buffer_Staging_createStagingBuffer"));

        // fill it with the vertices
        auto mapResult = stagingBuffer.memory->map(0, size);
        CHECK_KEEP(mapResult);
        HostMemoryMapping data = mapResult.take();

        memcpy(*data, sourceData, size);

        return std::move(stagingBuffer);
    }

    void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer,
        vk::raii::Image& image, u32 width, u32 height)
    {
        vk::BufferImageCopy region {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {width, height, 1}
        };

        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
    }
}