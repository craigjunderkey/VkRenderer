#include "../hello_triangle_application.h"
#include "uniform_buffer.h"
#include "buffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <numbers>

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createUniformBuffers(App& app)
    {
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            auto ubo = MappedBuffer::fromFatBufferResult(
                FatBuffer::create(
                    app,
                    sizeof(MVP),
                    vk::BufferUsageFlagBits::eUniformBuffer,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                )
            );
            
            if (ubo.isErr())
                return Err<std::string>(ubo.error());
            
            app.uniformBuffers.emplace_back(ubo.take());
        }

        return Ok();
    }

    void updateUniformBuffer(App& app, u32 frameIndex)
    {
        // calculate time
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        f32 time = std::chrono::duration<f32, std::chrono::seconds::period>(currentTime - startTime).count();

        // compute MVP
        constexpr f32 HALF_PI = std::numbers::pi / 2.0f;
        constexpr f32 QUARTER_PI = std::numbers::pi / 4.0f;

        MVP ubo {
            .model = glm::rotate(mat4(1.0f), time * HALF_PI, vec3(0.0f, 0.0f, 1.0f)),
            .view = glm::lookAt(vec3(2.0f, 2.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
            .projection = glm::perspective(
                QUARTER_PI,
                static_cast<f32>(app.swapChainExtent.width) / static_cast<f32>(app.swapChainExtent.height),
                0.1f, 10.0f
            )
        };
        ubo.projection[1][1] *= -1;

        // copy MVP to host memory
        memcpy(app.uniformBuffers[frameIndex].data, &ubo, sizeof(ubo));
    }
}