#include "../hello_triangle_application.h"
#include "command_buffer.h"
#include "../geometry/vertex.h"
#include "../geometry/mesh.h"
#include "push_constants.h"
#include "../render_loop/game_loop.h"
#include "command_buffer_helper.h"

#include <glm/gtx/string_cast.hpp>

using VkRenderer::RenderLoop::GameLoop;

using VkRenderer::App;

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createCommandBuffers(App& app)
    {
        vk::CommandBufferAllocateInfo allocInfo {
            .commandPool = **app.graphicsQueue.commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = MAX_FRAMES_IN_FLIGHT
        };

        auto allocResult = castVkResult(app.device.allocateCommandBuffers(allocInfo));
        CHECK_KEEP(allocResult);
        app.graphicsQueue.commandBuffers = allocResult.take();

        return Ok();
    }

    void beginRender(App& app, u32 imageIndex)
    {
        CommandBufferHelper cmdBuffer = CommandBufferHelper(app, app.graphicsQueue.commandBuffers[app.frameIndex]);
        
        cmdBuffer->begin({});

        // before we start rendering, transition resolve image to optimal layout
        // TODO: make FatSwapchain
        vk::ImageMemoryBarrier2 imageBarrier {
            .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .srcAccessMask = {}, // memory is already available; no writes to flush
            .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
            .oldLayout = vk::ImageLayout::eUndefined,
            .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .srcQueueFamilyIndex = app.graphicsQueue.familyIndex,
            .dstQueueFamilyIndex = app.graphicsQueue.familyIndex,
            .image = app.swapchain.getImages()[imageIndex],
            .subresourceRange = vk::ImageSubresourceRange {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        cmdBuffer->pipelineBarrier2(vk::DependencyInfo { .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &imageBarrier });

        // then transition color attachment image to optimal layout
        cmdBuffer.addImageBarrier(
            app.colorTexture,
            ImageState {
                .layout = vk::ImageLayout::eUndefined,
                .queueFamilyIndex = app.graphicsQueue.familyIndex,
                .accessMask = {},
                .stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput
            },
            ImageState {
                .layout = vk::ImageLayout::eColorAttachmentOptimal,
                .queueFamilyIndex = app.graphicsQueue.familyIndex,
                .accessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
                .stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput
            }
        );

        // then transition depth image to optimal layout
        cmdBuffer.addImageBarrier(
            app.depthTexture,
            ImageState {
                .layout = vk::ImageLayout::eUndefined,
                .queueFamilyIndex = app.graphicsQueue.familyIndex,
                .accessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
                .stageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests
            },
            ImageState {
                .layout = vk::ImageLayout::eDepthAttachmentOptimal,
                .queueFamilyIndex = app.graphicsQueue.familyIndex,
                .accessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
                .stageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests
            }
        );

        // set up the color attachment
        vk::ClearValue clearColor { .color = vk::ClearColorValue(CLEAR_COLOR) };
        vk::RenderingAttachmentInfo colorAttachment;

        if (App::CONFIG.graphics.antiAliasing == Setting::AntiAliasing::None)
        {
            colorAttachment = {
                .imageView = app.swapchain.getImageViews()[imageIndex],
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
                .resolveImageView = app.swapchain.getImageViews()[imageIndex],
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
                .extent = app.swapchain.getExtent()  
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
            .pDepthAttachment = &depthAttachment
        };

        // begin rendering
        cmdBuffer->beginRendering(renderingInfo);
        FatPipeline& scenePipeline = app.pipelines.get("scene");
        cmdBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, **scenePipeline);

        // viewport and scissor are dynamic, so we must set them here
        cmdBuffer->setViewport(0, vk::Viewport {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<f32>(app.swapchain.getExtent().width),
            .height = static_cast<f32>(app.swapchain.getExtent().height),
            .minDepth = 0.0f, .maxDepth = 1.0f
        });
        cmdBuffer->setScissor(
            0,
            vk::Rect2D {
                .offset = vk::Offset2D {.x = 0, .y = 0},
                .extent = app.swapchain.getExtent()
            }
        );

        // bind our descriptor sets
        cmdBuffer->bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, *scenePipeline.layout, 0,
            {
                *app.resources.bindless().getDescriptorSet(),
                *app.descriptorSets[app.frameIndex]
            },
            nullptr
        );
    }

    void drawPointLights(App& app, CommandBufferHelper& cmdBuffer)
    {
        // bind debug point light mesh
        const Geometry::Mesh& mesh = app.resources.get<Geometry::Mesh>("light_ball");
        cmdBuffer.bindMesh(mesh);
        
        // draw each point light
        for (u32 i = 0; i < app.environment.numPointLights; i++)
        {
            const PointLight& light = app.environment.pointLights[i];
            DebugLightPushConstants pushConstants {
                .mvp = app.projection * app.view * glm::translate(mat4(1.0f), light.posWorld),
                .color = vec4(light.components.diffuse, 1.0f),
            };
            cmdBuffer.pushConstants<DebugLightPushConstants>(
                pushConstants,
                *app.pipelines.get("debug_light").layout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
            );
            cmdBuffer.drawMesh(mesh);
        }
    }

    void drawSpotLights(App& app, CommandBufferHelper& cmdBuffer)
    {
        // bind debug spot light mesh
        const Geometry::Mesh& mesh = app.resources.get<Geometry::Mesh>("light_cone");
        cmdBuffer.bindMesh(mesh);

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
            cmdBuffer.pushConstants<DebugLightPushConstants>(
                pushConstants,
                *app.pipelines.get("debug_light").layout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
            );
            cmdBuffer.drawMesh(mesh);
        }
    }

    void drawLights(App& app, CommandBufferHelper& cmdBuffer)
    {
        // change to debug light pipeline
        cmdBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, **app.pipelines.get("debug_light"));
        
        // draw each light
        drawPointLights(app, cmdBuffer);
        drawSpotLights(app, cmdBuffer);
    }

    void drawSkybox(App& app, CommandBufferHelper& cmdBuffer)
    {
        // change to skybox pipeline
        cmdBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, **app.pipelines.get("skybox"));

        // draw cube
        const Geometry::Mesh& cube = app.resources.get<Geometry::Mesh>("skybox");
        cmdBuffer.bindMesh(cube);
        cmdBuffer.drawMesh(cube);
    }

    void endRender(App& app, u32 imageIndex)
    {
        CommandBufferHelper cmdBuffer = CommandBufferHelper(app, app.graphicsQueue.commandBuffers[app.frameIndex]);

        // draw the skybox
        drawSkybox(app, cmdBuffer);
        
        // draw all the lights
        drawLights(app, cmdBuffer);

        // stop rendering
        cmdBuffer->endRendering();

        // after rendering, transition the resolve image layout to one that can be presented
        vk::ImageMemoryBarrier2 imageBarrier {
            .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentRead, //vk::AccessFlagBits2::eColorAttachmentWrite
            .dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe,
            .dstAccessMask = {},
            .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .newLayout = vk::ImageLayout::ePresentSrcKHR,
            .srcQueueFamilyIndex = app.graphicsQueue.familyIndex,
            .dstQueueFamilyIndex = app.graphicsQueue.familyIndex,
            .image = app.swapchain.getImages()[imageIndex],
            .subresourceRange = vk::ImageSubresourceRange {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        cmdBuffer->pipelineBarrier2(vk::DependencyInfo { .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &imageBarrier });
 
        cmdBuffer->end();
    }

    Result<vk::raii::CommandBuffer, ErrorMessage> beginSingleTimeCommands(App& app, FatQueue& queue)
    {
        vk::CommandBufferAllocateInfo allocInfo {
            .commandPool = *queue.commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1
        };

        auto buffersResult = castVkResult(app.device.allocateCommandBuffers(allocInfo));
        CHECK_KEEP(buffersResult);
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
}