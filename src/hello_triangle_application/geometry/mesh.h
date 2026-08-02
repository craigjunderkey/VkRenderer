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
    };
    
    struct Mesh
    {
        StringId name = StringId();
        std::vector<Vertex> vertices = {};
        std::vector<u32> indices = {};
        u32 numIndices;
        FatBuffer vertexBuffer = {};
        FatBuffer indexBuffer = {};

        static Mesh fromData(MeshData data)
        {
            Mesh mesh {
                .vertices = std::move(data.vertices),
                .indices = std::move(data.indices)
            };
            return mesh;
        }

        Result<Unit, std::string> makeBuffers(App& app)
        {
            // families that can access the buffers
            std::vector<u32> families = meshQueueFamilies(app);

            // create vertex buffer
            FatBuffer stagingBuffer;
            vk::DeviceSize verticesSize = sizeof(this->vertices[0]) * this->vertices.size();
            TRY_ASSIGN(stagingBuffer, createStagingBuffer(app, (void*)this->vertices.data(), verticesSize));
            TRY_ASSIGN(this->vertexBuffer,
                FatBuffer::create(
                    app, verticesSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                    vk::MemoryPropertyFlagBits::eDeviceLocal, families
                )
            );
            CHECK(copyBuffer(app, stagingBuffer, this->vertexBuffer));

            // create index buffer
            this->numIndices = this->indices.size();
            vk::DeviceSize indicesSize = sizeof(this->indices[0]) * this->indices.size();
            TRY_ASSIGN(stagingBuffer, createStagingBuffer(app, (void*)this->indices.data(), indicesSize));
            TRY_ASSIGN(this->indexBuffer,
                FatBuffer::create(
                    app, indicesSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                    vk::MemoryPropertyFlagBits::eDeviceLocal, families
                )
            );
            CHECK(copyBuffer(app, stagingBuffer, this->indexBuffer));

            // drop vertices and indices from main memory
            this->vertices = {};
            this->indices = {};

            return Ok();
        }
    };
}