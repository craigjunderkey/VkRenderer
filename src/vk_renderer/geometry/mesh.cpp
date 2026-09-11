#include "../vk_renderer.h"
#include "mesh.h"
#include "vertex_buffer.h"

namespace VkRenderer::Geometry
{
    std::vector<u32> Mesh::queueFamilies(App& app)
    {
        return { app.graphicsQueue.familyIndex, app.transferQueue.familyIndex };
    }

    Result<Unit, ErrorMessage> Mesh::makeBuffers(App& app)
    {
        // families that can access the buffers
        std::vector<u32> families = Mesh::queueFamilies(app);

        // create vertex buffer
        vk::DeviceSize verticesSize = sizeof(this->data.vertices[0]) * this->data.vertices.size();
        
        auto stagingBufferResult = createStagingBuffer(app, static_cast<void*>(this->data.vertices.data()), verticesSize);
        CHECK_KEEP(stagingBufferResult);
        FatBuffer stagingBuffer = stagingBufferResult.take();
        
        auto vertexBufferResult = FatBuffer::create(
            app, verticesSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal, families
        );
        CHECK_KEEP(vertexBufferResult);
        this->vertexBuffer = vertexBufferResult.take();

        CHECK(copyBuffer(app, stagingBuffer, this->vertexBuffer));

        // create index buffer
        assert(this->data.numIndices == this->data.indices.size());
        vk::DeviceSize indicesSize = sizeof(this->data.indices[0]) * this->data.indices.size();
        auto indexStageResult = createStagingBuffer(app, static_cast<void*>(this->data.indices.data()), indicesSize);
        CHECK_KEEP(indexStageResult);
        stagingBuffer = indexStageResult.take();

        auto indexBufferResult = FatBuffer::create(
            app, indicesSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal, families
        );
        CHECK_KEEP(indexBufferResult);
        this->indexBuffer = indexBufferResult.take();

        CHECK(copyBuffer(app, stagingBuffer, this->indexBuffer));

        // drop vertices and indices from main memory
        this->data.vertices = {};
        this->data.indices = {};

        return Ok();
    }
}