#pragma once
#include "../utilities/common.h"
#include "vertex.h"
#include "buffer.h"
#include "vertex_buffer.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    std::vector<u32> meshQueueFamilies(App& app);

    struct MeshData
    {
        std::vector<Vertex> vertices = {};
        std::vector<u32> indices = {};
        u32 numIndices = 0;
    };
    
    struct Mesh
    {
        StringId name = StringId();
        MeshData data = {};
        FatBuffer vertexBuffer = {};
        FatBuffer indexBuffer = {};

        Result<Unit, std::string> makeBuffers(App& app)
        {
            // families that can access the buffers
            std::vector<u32> families = meshQueueFamilies(app);

            // create vertex buffer
            FatBuffer stagingBuffer;
            vk::DeviceSize verticesSize = sizeof(this->data.vertices[0]) * this->data.vertices.size();
            TRY_ASSIGN(stagingBuffer, createStagingBuffer(app, (void*)this->data.vertices.data(), verticesSize));
            TRY_ASSIGN(this->vertexBuffer,
                FatBuffer::create(
                    app, verticesSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                    vk::MemoryPropertyFlagBits::eDeviceLocal, families
                )
            );
            CHECK(copyBuffer(app, stagingBuffer, this->vertexBuffer));

            // create index buffer
            assert(this->data.numIndices == this->data.indices.size());
            vk::DeviceSize indicesSize = sizeof(this->data.indices[0]) * this->data.indices.size();
            TRY_ASSIGN(stagingBuffer, createStagingBuffer(app, (void*)this->data.indices.data(), indicesSize));
            TRY_ASSIGN(this->indexBuffer,
                FatBuffer::create(
                    app, indicesSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                    vk::MemoryPropertyFlagBits::eDeviceLocal, families
                )
            );
            CHECK(copyBuffer(app, stagingBuffer, this->indexBuffer));

            // drop vertices and indices from main memory
            this->data.vertices = {};
            this->data.indices = {};

            return Ok();
        }
    };
}