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
    Result<FatBuffer, std::string> createStagingBuffer(App& app, void* sourceData, vk::DeviceSize size)
    {
        std::vector<u32> families = {
            app.graphicsQueue.familyIndex,
            app.transferQueue.familyIndex
        };

        // create the staging buffer
        FatBuffer stagingBuffer;
        TRY_ASSIGN(
            stagingBuffer,
            FatBuffer::create(
                app,
                size,
                vk::BufferUsageFlagBits::eTransferSrc,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                std::move(families)
            )
        );

        // fill it with the vertices
        void* data;
        TRY_ASSIGN(
            data,
            castVkResult(stagingBuffer.memory.mapMemory(0, size))
        );

        memcpy(data, sourceData, size);

        stagingBuffer.memory.unmapMemory();

        return std::move(stagingBuffer);
    }

    Result<Unit, std::string> copyBuffer(App& app, FatBuffer& src, FatBuffer& dst)
    {
        vk::raii::CommandBuffer copyCommand = nullptr;
        TRY_ASSIGN(
            copyCommand,
            beginSingleTimeCommands(app, app.transferQueue)
        );

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