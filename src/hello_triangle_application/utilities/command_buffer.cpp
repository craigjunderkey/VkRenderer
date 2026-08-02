#include "../hello_triangle_application.h"
#include "command_buffer.h"
#include "../geometry/vertex.h"
#include "../geometry/mesh.h"
#include "push_constants.h"
#include "../render_loop/game_loop.h"

#include <glm/gtx/string_cast.hpp>

using HelloTriangleApplication::RenderLoop::GameLoop;

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Utilities
{
    void stageTransitionImageLayout(
        App& app,
        const vk::Image& image,
        vk::raii::CommandBuffer& commandBuffer,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::AccessFlags2 srcAccessMask,
        vk::AccessFlags2 dstAccessMask,
        vk::PipelineStageFlags2 srcStageMask,
        vk::PipelineStageFlags2 dstStageMask,
        vk::ImageAspectFlags imageAspectFlags
    )
    {
        vk::ImageMemoryBarrier2 barrier {
            .srcStageMask = srcStageMask,
            .srcAccessMask = srcAccessMask,
            .dstStageMask = dstStageMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = image,
            .subresourceRange = {
                .aspectMask = imageAspectFlags,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        vk::DependencyInfo dependencyInfo {
            .dependencyFlags = {},
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrier,
        };

        commandBuffer.pipelineBarrier2(dependencyInfo);
    }
    
    Result<Unit, std::string> createCommandBuffers(App& app)
    {
        vk::CommandBufferAllocateInfo allocInfo {
            .commandPool = **app.graphicsQueue.commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = MAX_FRAMES_IN_FLIGHT
        };

        TRY_ASSIGN(
            app.graphicsQueue.commandBuffers,
            castVkResult(app.device.allocateCommandBuffers(allocInfo))
        );

        return Ok();
    }

    void beginRender(App& app, u32 imageIndex)
    {
        const vk::raii::CommandBuffer& commandBuffer = app.graphicsQueue.commandBuffers[app.frameIndex];
        
        commandBuffer.begin({});

        // before we start rendering, color attachment image to optimal layout
        stageTransitionImageLayout(
            app,
            app.swapChainImages[imageIndex],
            app.graphicsQueue.commandBuffers[app.frameIndex],
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::ImageAspectFlagBits::eColor
        );

        // then transition multisampled color attachment image to optimal layout
        stageTransitionImageLayout(
            app,
            app.colorTexture.image,
            app.graphicsQueue.commandBuffers[app.frameIndex],
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::ImageAspectFlagBits::eColor
        );

        // then transition depth image to optimal layout
        stageTransitionImageLayout(
            app, app.depthTexture.image, app.graphicsQueue.commandBuffers[app.frameIndex],
            vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal,
            vk::AccessFlagBits2::eDepthStencilAttachmentWrite, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
            vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
            vk::ImageAspectFlagBits::eDepth
        );

        // set up the color attachment
        vk::ClearValue clearColor { .color = vk::ClearColorValue(CLEAR_COLOR) };
        vk::RenderingAttachmentInfo colorAttachment;

        if (CONFIG.antiAliasing == Setting::AntiAliasing::None)
        {
            colorAttachment = {
                .imageView = app.swapChainImageViews[imageIndex],
                .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
                .loadOp = vk::AttachmentLoadOp::eClear,
                .storeOp = vk::AttachmentStoreOp::eStore,
                .clearValue = clearColor
            };
        }
        else
        {
            colorAttachment = {
                .imageView = app.colorTexture.view,
                .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
                .resolveMode = vk::ResolveModeFlagBits::eAverage,
                .resolveImageView = app.swapChainImageViews[imageIndex],
                .resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal,
                .loadOp = vk::AttachmentLoadOp::eClear,
                .storeOp = vk::AttachmentStoreOp::eDontCare,
                .clearValue = clearColor
            };
        }
        
        // set up the depth attachment
        vk::ClearValue clearDepth { .depthStencil = {1.0f, 0} };
        vk::RenderingAttachmentInfo depthAttachment {
            .imageView = app.depthTexture.view,
            .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eDontCare,
            .clearValue = clearDepth
        };

        // set up rendering info
        vk::RenderingInfo renderingInfo {
            .renderArea = {
                .offset = {0, 0},
                .extent = app.swapChainExtent  
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
            .pDepthAttachment = &depthAttachment
        };

        // begin rendering
        commandBuffer.beginRendering(renderingInfo);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, **app.pipeline);

        // viewport and scissor are dynamic, so we must set them here
        commandBuffer.setViewport(0, vk::Viewport {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<f32>(app.swapChainExtent.width),
            .height = static_cast<f32>(app.swapChainExtent.height),
            .minDepth = 0.0f, .maxDepth = 1.0f
        });
        commandBuffer.setScissor(
            0,
            vk::Rect2D {
                .offset = vk::Offset2D {.x = 0, .y = 0},
                .extent = app.swapChainExtent
            }
        );

        // bind our descriptor sets
        commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, *app.pipeline.layout, 0,
            {
                *app.resources.materialManager().descriptorSet(),
                *app.descriptorSets[app.frameIndex]
            },
            nullptr
        );
    }

    void bindMesh(const vk::raii::CommandBuffer& commandBuffer, const Geometry::Mesh& mesh)
    {
        commandBuffer.bindVertexBuffers(0, *mesh.vertexBuffer.buffer, {0});
        commandBuffer.bindIndexBuffer(*mesh.indexBuffer.buffer, 0, vk::IndexTypeValue<decltype(mesh.data.indices)::value_type>::value);
    }

    void drawPointLights(App& app, const vk::raii::CommandBuffer& commandBuffer)
    {
        // bind debug point light mesh
        const Geometry::Mesh& mesh = app.resources.mesh("light_ball");
        bindMesh(commandBuffer, mesh);
        
        // draw each point light
        for (u32 i = 0; i < app.environment.numPointLights; i++)
        {
            const PointLight& light = app.environment.pointLights[i];
            DebugLightPushConstants pushConstants {
                .mvp = app.projection * app.view * glm::translate(mat4(1.0f), light.posWorld),
                .color = vec4(light.components.diffuse, 1.0f),
            };
            commandBuffer.pushConstants(
                *app.debugLightPipeline.layout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0, sizeof(DebugLightPushConstants), &pushConstants
            );
            commandBuffer.drawIndexed(mesh.data.numIndices, 1, 0, 0, 0);
        }
    }

    void drawSpotLights(App& app, const vk::raii::CommandBuffer& commandBuffer)
    {
        // bind debug spot light mesh
        const Geometry::Mesh& mesh = app.resources.mesh("light_cone");
        bindMesh(commandBuffer, mesh);

        // draw each spot light
        for (u32 i = 0; i < app.environment.numSpotLights; i++)
        {
            const SpotLight& light = app.environment.spotLights[i];
            
            // model matrix follows light direction and uses x+ for orientation
            mat4 model = mat4(1.0f);
            vec3 right = vec3(1.0f, 0.0f, 0.0f);
            model[0] = vec4(right, 0.0f);
            model[1] = vec4(glm::cross(right, -light.dirWorld), 0.0f);
            model[2] = vec4(-light.dirWorld, 0.0f);
            model[3] = vec4(light.posWorld, 1.0f);
            
            DebugLightPushConstants pushConstants {
                .mvp = app.projection * app.view * model,
                .color = vec4(light.components.diffuse, 1.0f)
            };
            commandBuffer.pushConstants(
                *app.debugLightPipeline.layout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0, sizeof(DebugLightPushConstants), &pushConstants
            );
            commandBuffer.drawIndexed(mesh.data.numIndices, 1, 0, 0, 0);
        }
    }

    void drawLights(App& app, const vk::raii::CommandBuffer& commandBuffer)
    {
        // change to debug light pipeline
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, **app.debugLightPipeline);
        
        // draw each light
        drawPointLights(app, commandBuffer);
        drawSpotLights(app, commandBuffer);
    }

    void endRender(App& app, u32 imageIndex)
    {
        const vk::raii::CommandBuffer& commandBuffer = app.graphicsQueue.commandBuffers[app.frameIndex];

        // draw all the lights
        drawLights(app, commandBuffer);

        // stop rendering
        commandBuffer.endRendering();

        // after rendering, transition the image layout to one that can be presented
        stageTransitionImageLayout(
            app,
            app.swapChainImages[imageIndex],
            app.graphicsQueue.commandBuffers[app.frameIndex],
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe,
            vk::ImageAspectFlagBits::eColor
        );

        commandBuffer.end();
    }

    Result<vk::raii::CommandBuffer, std::string> beginSingleTimeCommands(App& app, FatQueue& queue)
    {
        vk::CommandBufferAllocateInfo allocInfo {
            .commandPool = *queue.commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1
        };

        auto buffersResult = castVkResult(app.device.allocateCommandBuffers(allocInfo));
        if (!buffersResult)
            return Err<std::string>(buffersResult.error());
        vk::raii::CommandBuffer commandBuffer = std::move(buffersResult.take().front());

        vk::CommandBufferBeginInfo beginInfo {
            .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        };
        commandBuffer.begin(beginInfo);

        return std::move(commandBuffer);
    }

    void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer, FatQueue& queue)
    {
        commandBuffer.end();
        vk::SubmitInfo submitInfo {
            .commandBufferCount = 1,
            .pCommandBuffers = &*commandBuffer
        };
        queue.queue.submit(submitInfo, nullptr);
        queue.queue.waitIdle();
    }

    Result<Unit, std::string> transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::Image& image,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout, u32 mipLevels, u32 srcFamily, u32 dstFamily)
    {
        vk::ImageMemoryBarrier barrier {
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = image,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .levelCount = mipLevels,
                .layerCount = 1
            }
        };

        vk::PipelineStageFlags sourceStage, destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage      = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else
        {
            return Err<std::string>("Unsupported layout transition.");
        }

        commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);

        return Ok();
    }
}