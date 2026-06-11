#include "../hello_triangle_application.h"
#include "vertex_buffer.h"
#include "vertex.h"
#include "buffer.h"

using HelloTriangleApplication::App;
using HelloTriangleApplication::Geometry::Vertex;
using HelloTriangleApplication::Geometry::FatBuffer;

Result<Unit, std::string> fillBuffer(FatBuffer& buffer, const std::vector<Vertex>& vertices)
{
    void* data;
    TRY_ASSIGN(
        data,
        castVkResult(buffer.memory.mapMemory(0, buffer.size))
    );

    memcpy(data, vertices.data(), buffer.size);

    buffer.memory.unmapMemory();

    return Ok();
}

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createVertexBuffer(App& app)
    {
        std::vector<u32> families = {
            app.graphicsQueue.familyIndex,
            app.transferQueue.familyIndex
        };

        FatBuffer stagingBuffer;
        TRY_ASSIGN(
            stagingBuffer,
            FatBuffer::create(
                app,
                sizeof(VERTICES[0]) * VERTICES.size(),
                vk::BufferUsageFlagBits::eVertexBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                std::move(families)
            )
        );

        // vk::BufferCreateInfo bufferCreateInfo {
        //     .size = sizeof(VERTICES[0]) * VERTICES.size(),
        //     .usage = vk::BufferUsageFlagBits::eVertexBuffer,
        //     .sharingMode = vk::SharingMode::eConcurrent,
        //     .queueFamilyIndexCount = (u32)app.vertexBufferFamilies.size(),
        //     .pQueueFamilyIndices = app.vertexBufferFamilies.data()
        // };

        // TRY_ASSIGN(
        //     app.vertexBuffer,
        //     castVkResult(app.device.createBuffer(bufferCreateInfo))
        // );

        // CHECK(allocateMemory(app));
        CHECK(fillBuffer(stagingBuffer, VERTICES));

        app.vertexBuffer = std::move(stagingBuffer);

        return Ok();
    }
}