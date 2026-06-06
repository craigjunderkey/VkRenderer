#include "../hello_triangle_application.h"
#include "command_buffer.h"

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

    app.commandBuffer.pipelineBarrier2(dependencyInfo);
}

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createCommandBuffer(App& app)
    {
        vk::CommandBufferAllocateInfo allocInfo {
            .commandPool = app.commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1
        };

        std::vector<vk::raii::CommandBuffer> commandBuffers;
        TRY_ASSIGN(commandBuffers, castVkResult(app.device.allocateCommandBuffers(allocInfo)));
        app.commandBuffer = std::move(commandBuffers.front());

        return Ok();
    }

    void recordCommandBuffer(App& app, u32 imageIndex)
    {
        app.commandBuffer.begin({});

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
        std::array<f32, 4> color {0.0f, 0.0f, 0.0f, 1.0f};
        vk::ArrayWrapper1D<f32, 4> clearColor(color);
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
        app.commandBuffer.beginRendering(renderingInfo);
        app.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *app.graphicsPipeline);

        // viewport and scissor are dynamic, so we must set them here
        app.commandBuffer.setViewport(0, vk::Viewport {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<f32>(app.swapChainExtent.width),
            .height = static_cast<f32>(app.swapChainExtent.height),
            .minDepth = 0.0f, .maxDepth = 1.0f
        });
        app.commandBuffer.setScissor(
            0,
            vk::Rect2D {
                .offset = vk::Offset2D {.x = 0, .y = 0},
                .extent = app.swapChainExtent
            }
        );

        // draw the damn thing
        app.commandBuffer.draw(3, 1, 0, 0);

        // stop rendering
        app.commandBuffer.endRendering();

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

        app.commandBuffer.end();
    }
}