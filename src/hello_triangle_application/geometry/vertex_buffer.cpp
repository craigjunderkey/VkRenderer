#include "../hello_triangle_application.h"
#include "vertex_buffer.h"
#include "vertex.h"
#include "buffer.h"

using HelloTriangleApplication::App;
using HelloTriangleApplication::Geometry::Vertex;
using HelloTriangleApplication::Geometry::FatBuffer;
using HelloTriangleApplication::Geometry::VERTICES;

Result<Unit, std::string> createStagingBuffer(App& app)
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
            sizeof(VERTICES[0]) * VERTICES.size(),
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            std::move(families)
        )
    );

    // fill it with the vertices
    void* data;
    TRY_ASSIGN(
        data,
        castVkResult(stagingBuffer.memory.mapMemory(0, stagingBuffer.size))
    );

    memcpy(data, VERTICES.data(), stagingBuffer.size);

    stagingBuffer.memory.unmapMemory();

    // store it in the app
    app.stagingBuffer = std::move(stagingBuffer);

    return Ok();
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
        CHECK(createStagingBuffer(app));
        CHECK(createVertexBuffer(app));
        CHECK(copyBuffer(app, app.stagingBuffer, app.vertexBuffer));
        return Ok();
    }
}