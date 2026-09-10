#include "../hello_triangle_application.h"
#include "uniform_buffer.h"
#include "buffer.h"

namespace VkRenderer::Geometry
{
    Result<Unit, ErrorMessage> createPerFrameUbos(App& app)
    {
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            auto ubo = MappedBuffer::fromBufferResult(
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

    Result<Unit, ErrorMessage> updatePerFrameUbo(App& app, u32 frameIndex)
    {
        PerFrameInfo ubo {
            .view = app.view,
            .projection = app.projection,
            .env = app.environment
        };
    
        memcpy(*app.uniformBuffers[app.frameIndex].mapping, &ubo, sizeof(ubo));
        
        return Ok();
    }
}