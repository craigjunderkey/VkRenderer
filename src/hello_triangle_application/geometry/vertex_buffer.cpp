#include "../hello_triangle_application.h"
#include "vertex_buffer.h"
#include "vertex.h"
#include "buffer.h"
#include "../utilities/command_buffer.h"

using HelloTriangleApplication::App;
using HelloTriangleApplication::Geometry::Vertex;
using HelloTriangleApplication::Geometry::FatBuffer;

using namespace HelloTriangleApplication::Utilities;

namespace HelloTriangleApplication::Geometry
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

        // fill it with the vertices
        auto mapResult = stagingBuffer.memory.map(0, size);
        CHECK_KEEP(mapResult);
        HostMemoryMapping data = mapResult.take();

        memcpy(*data, sourceData, size);

        return std::move(stagingBuffer);
    }

    Result<Unit, ErrorMessage> copyBuffer(App& app, FatBuffer& src, FatBuffer& dst)
    {
        auto beginResult = beginSingleTimeCommands(app, app.transferQueue);
        CHECK_KEEP(beginResult);
        vk::raii::CommandBuffer copyCommand = beginResult.take();

        copyCommand.copyBuffer(*src.buffer, *dst.buffer, vk::BufferCopy { .size = src.size } ); 
        
        endSingleTimeCommands(std::move(copyCommand), app.transferQueue);

        return Ok();
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