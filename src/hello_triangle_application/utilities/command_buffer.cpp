#include "../hello_triangle_application.h"
#include "command_buffer.h"
#include "../geometry/vertex.h"

using HelloTriangleApplication::App;

void transitionImageLayout(
    App& app,
    u32 imageIndex,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    vk::AccessFlags2 srcAccessMask,
    vk::AccessFlags2 dstAccessMask,
    vk::PipelineStageFlags2 srcStageMask,
    vk::PipelineStageFlags2 dstStageMask
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
        .image = app.swapChainImages[imageIndex],
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
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

    app.commandBuffers[app.frameIndex].pipelineBarrier2(dependencyInfo);
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createCommandBuffers(App& app)
    {
        vk::CommandBufferAllocateInfo allocInfo {
            .commandPool = app.commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = MAX_FRAMES_IN_FLIGHT
        };

        TRY_ASSIGN(
            app.commandBuffers,
            castVkResult(app.device.allocateCommandBuffers(allocInfo))
        );

        return Ok();
    }

    void recordCommandBuffer(App& app, u32 imageIndex)
    {
        const vk::raii::CommandBuffer& commandBuffer = app.commandBuffers[app.frameIndex];
        
        commandBuffer.begin({});

        // before we start rendering, transition to color attachment optimal image layout
        transitionImageLayout(
            app,
            imageIndex,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        // set up the color attachment
        vk::ArrayWrapper1D<f32, 4> clearColor(CLEAR_COLOR);
        vk::ClearColorValue clearColorValue(std::move(clearColor));
        vk::ClearValue clearValue { .color = std::move(clearColorValue) };

        vk::RenderingAttachmentInfo attachmentInfo {
            .imageView = app.swapChainImageViews[imageIndex],
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .clearValue = clearValue
        };

        // set up rendering info
        vk::RenderingInfo renderingInfo {
            .renderArea = {
                .offset = {0, 0},
                .extent = app.swapChainExtent  
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachmentInfo
        };

        // begin rendering
        commandBuffer.beginRendering(renderingInfo);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *app.graphicsPipeline);

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

        // draw the damn thing
        commandBuffer.bindVertexBuffers(0, *app.vertexBuffer, {0});
        commandBuffer.draw(Geometry::VERTICES.size(), 1, 0, 0);

        // stop rendering
        commandBuffer.endRendering();

        // after rendering, transition the image layout to one that can be presented
        transitionImageLayout(
            app,
            imageIndex,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe
        );

        commandBuffer.end();
    }
}