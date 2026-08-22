#include "../hello_triangle_application.h"
#include "uniform_buffer.h"
#include "buffer.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, ErrorMessage> createUniformBuffers(App& app)
    {
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            auto ubo = MappedBuffer::fromFatBufferResult(
                FatBuffer::create(
                    app,
                    sizeof(PerFrameInfo),
                    vk::BufferUsageFlagBits::eUniformBuffer,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                )
            );
            CHECK_KEEP(ubo);            
            app.uniformBuffers.emplace_back(ubo.take());
        }
        return Ok();
    }

    void updateUniformBuffer(App& app, u32 frameIndex)
    {
        PerFrameInfo ubo {
            .view = app.view,
            .projection = app.projection,
            .env = app.environment
        };

        memcpy(app.uniformBuffers[frameIndex].data, &ubo, sizeof(ubo));
    }
}