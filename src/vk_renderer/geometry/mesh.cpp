#include "../vk_renderer.h"
#include "mesh.h"
#include "vertex_buffer.h"
#include "../utilities/staged_copy.h"
#include "../utilities/vulkan_debug.h"

using namespace VkRenderer::Utilities;

namespace VkRenderer::Geometry
{
    std::vector<u32> Mesh::queueFamilies(App& app)
    {
        return { app.graphicsQueue.familyIndex, app.transferQueue.familyIndex };
    }

    Result<Unit, ErrorMessage> Mesh::makeBuffers(App& app)
    {
        // create vertex buffer
        auto vertexBufferResult = [&app = app, &vertices = this->data.vertices] -> Result<FatBuffer, ErrorMessage>
        {
            vk::DeviceSize size = sizeof(vertices[0]) * vertices.size();

            auto vertexBufferResult = FatBuffer::create(
                app, size,
                vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                vk::MemoryPropertyFlagBits::eDeviceLocal
            );
            CHECK_KEEP(vertexBufferResult);
            FatBuffer vertexBuffer = vertexBufferResult.take();

            auto copy = StagedCopy(app);
            CHECK(copy.begin(vertices.data(), size)
                .addDst(*vertexBuffer)
                .end());

            return vertexBuffer;
        }();
        CHECK_KEEP(vertexBufferResult);
        this->vertexBuffer = vertexBufferResult.take();
        this->data.vertices = {};

#ifndef NDEBUG
        {
            std::string* debugName = new std::string();
            *debugName = std::format("Buffer_Vertex_{}", StringId::getString(this->name));
            setDebugName(app, **this->vertexBuffer, debugName->c_str());
        }
#endif

        // create index buffer
        assert(this->data.numIndices == this->data.indices.size());
        
        auto indexBufferResult = [&app = app, &indices = this->data.indices] -> Result<FatBuffer, ErrorMessage>
        {
            vk::DeviceSize size = sizeof(indices[0]) * indices.size();

            auto indexBufferResult = FatBuffer::create(
                app, size,
                vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                vk::MemoryPropertyFlagBits::eDeviceLocal
            );
            CHECK_KEEP(indexBufferResult);
            FatBuffer indexBuffer = indexBufferResult.take();

            auto copy = StagedCopy(app);
            CHECK(copy.begin(indices.data(), size)
                .addDst(*indexBuffer)
                .end());

            return indexBuffer;
        }();
        CHECK_KEEP(indexBufferResult);
        this->indexBuffer = indexBufferResult.take();
        this->data.indices = {};

#ifndef NDEBUG
        {
            std::string* debugName = new std::string();
            *debugName = std::format("Buffer_Index_{}", StringId::getString(this->name));
            setDebugName(app, **this->indexBuffer, debugName->c_str());
        }
#endif

        return Ok();
    }
}