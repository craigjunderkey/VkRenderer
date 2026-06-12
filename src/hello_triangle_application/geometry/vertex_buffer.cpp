#include "../hello_triangle_application.h"
#include "vertex_buffer.h"
#include "vertex.h"
#include "buffer.h"

using HelloTriangleApplication::App;
using HelloTriangleApplication::Geometry::Vertex;
using HelloTriangleApplication::Geometry::FatBuffer;
using HelloTriangleApplication::Geometry::VERTICES;
using HelloTriangleApplication::Geometry::INDICES;

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
            sizeof(VERTICES[0]) * VERTICES.size(),
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            std::move(families)
        )
    );

    // store it in the app
    app.vertexBuffer = std::move(vertexBuffer);

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
            sizeof(u16) * INDICES.size(),
            vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            std::move(families)
        )
    );

    // store it in the app
    app.indexBuffer = std::move(indexBuffer);

    return Ok();
}

Result<Unit, std::string> copyBuffer(App& app, FatBuffer& src, FatBuffer& dst)
{
    vk::CommandBufferAllocateInfo allocInfo {
        .commandPool = *app.transferQueue.commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };

    auto copyBufferResult = castVkResult(app.device.allocateCommandBuffers(allocInfo));
    CHECK_KEEP(copyBufferResult);
    vk::raii::CommandBuffer copyCommand = std::move(copyBufferResult.take().front());
    
    copyCommand.begin( { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit } );
    copyCommand.copyBuffer(*src.buffer, *dst.buffer, vk::BufferCopy { .size = src.size } ); 
    copyCommand.end();

    app.transferQueue.queue.submit(
        vk::SubmitInfo {
            .commandBufferCount = 1,
            .pCommandBuffers = &*copyCommand,
        }
    );
    app.transferQueue.queue.waitIdle();

    return Ok();
}

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createBuffers(App& app)
    {
        // create vertex buffer
        FatBuffer stagingBuffer;
        TRY_ASSIGN(
            stagingBuffer,
            createStagingBuffer(app, (void*)VERTICES.data(), sizeof(VERTICES[0]) * VERTICES.size())
        );
        CHECK(createVertexBuffer(app));
        CHECK(copyBuffer(app, stagingBuffer, app.vertexBuffer));

        // create the index buffer
        TRY_ASSIGN(
            stagingBuffer,
            createStagingBuffer(app, (void*)INDICES.data(), sizeof(u16) * INDICES.size())
        );
        CHECK(createIndexBuffer(app));
        CHECK(copyBuffer(app, stagingBuffer, app.indexBuffer));

        return Ok();
    }
}