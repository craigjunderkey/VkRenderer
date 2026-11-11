#include "../vk_renderer.h"
#include "uniform_buffer.h"
#include "buffer.h"
#include "../utilities/vulkan_debug.h"

namespace VkRenderer::Geometry
{
    using namespace Utilities;

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
        
#ifndef NDEBUG
            std::string *debugName = new std::string();
            *debugName = std::format("Buffer_PerFrameUbo{}", i);
            setDebugName(app, **app.uniformBuffers.back(), debugName->c_str());
#endif
        }
        return Ok();
    }

    Result<Unit, ErrorMessage> updatePerFrameUbo(App& app, u32 frameIndex)
    {
        PerFrameInfo ubo {
            .view = app.view,
            .projection = app.projection,
            .env = app.environment,
            .dt = FRAME_TIME_SEC
        };
    
        memcpy(*app.uniformBuffers[app.frameIndex].mapping, &ubo, sizeof(ubo));
        
        return Ok();
    }
}