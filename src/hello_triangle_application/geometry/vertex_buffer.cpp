#include "../hello_triangle_application.h"
#include "vertex_buffer.h"
#include "vertex.h"
#include "buffer.h"
#include "../utilities/command_buffer.h"

using HelloTriangleApplication::App;
using HelloTriangleApplication::Geometry::Vertex;
using HelloTriangleApplication::Geometry::FatBuffer;
using HelloTriangleApplication::Geometry::VERTICES;
using HelloTriangleApplication::Geometry::INDICES;

using namespace HelloTriangleApplication::Utilities;

Result<Unit, std::string> createVertexBuffer(App& app)
{
    std::vector<u32> families = {
        app.graphicsQueue.familyIndex,
        app.transferQueue.familyIndex
    };

    // create the vertex buffer
    FatBuffer vertexBuffer;
    TRY_ASSIGN(
        vertexBuffer,
        FatBuffer::create(
            app,
            sizeof(app.mesh.vertices[0]) * app.mesh.vertices.size(),
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            std::move(families)
        )
    );

    // store it in the app
    app.mesh.vertexBuffer = std::move(vertexBuffer);

    return Ok();
}

Result<Unit, std::string> createIndexBuffer(App& app)
{
    std::vector<u32> families = {
        app.graphicsQueue.familyIndex,
        app.transferQueue.familyIndex
    };

    // create the index buffer
    FatBuffer indexBuffer;
    TRY_ASSIGN(
        indexBuffer,
        FatBuffer::create(
            app,
            sizeof(app.mesh.indices[0]) * app.mesh.indices.size(),
            vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            std::move(families)
        )
    );

    // store it in the app
    app.mesh.indexBuffer = std::move(indexBuffer);

    return Ok();
}

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

    Result<Unit, std::string> createBuffers(App& app)
    {
        // create vertex buffer
        FatBuffer stagingBuffer;
        TRY_ASSIGN(
            stagingBuffer,
            createStagingBuffer(app, (void*)app.mesh.vertices.data(), sizeof(app.mesh.vertices[0]) * app.mesh.vertices.size())
        );
        CHECK(createVertexBuffer(app));
        CHECK(copyBuffer(app, stagingBuffer, app.mesh.vertexBuffer));

        // create the index buffer
        TRY_ASSIGN(
            stagingBuffer,
            createStagingBuffer(app, (void*)app.mesh.indices.data(), sizeof(app.mesh.indices[0]) * app.mesh.indices.size())
        );
        CHECK(createIndexBuffer(app));
        CHECK(copyBuffer(app, stagingBuffer, app.mesh.indexBuffer));

        return Ok();
    }
}