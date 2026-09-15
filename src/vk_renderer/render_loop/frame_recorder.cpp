#include "../vk_renderer.h"
#include "frame_recorder.h"
#include <fstream>

using namespace VkRenderer::Geometry;

namespace VkRenderer::RenderLoop
{
    Result<Unit, ErrorMessage> FrameRecorder::captureFrame(App& app, vk::raii::CommandBuffer& cmdBuffer, vk::Image swapchainImage)
    {
        assert(this->isFrameCaptureQueued_);
        
        vk::BufferImageCopy copyRegion {
            .imageSubresource = vk::ImageSubresourceLayers {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .layerCount = 1
            },
            .imageExtent = vk::Extent3D {
                .width = app.swapchain.getExtent().width,
                .height = app.swapchain.getExtent().height,
                .depth = 1
            }
        };

        cmdBuffer.copyImageToBuffer(
            swapchainImage,
            vk::ImageLayout::eTransferSrcOptimal,
            **this->buffers_[app.frameIndex].buffer,
            copyRegion
        );

        this->isFrameCaptureQueued_ = false;
        this->hasCapturedFrame_[app.frameIndex] = true;
        return Ok();
    }

    Result<Unit, ErrorMessage> FrameRecorder::initialize(App& app, u32 width, u32 height)
    {
        CHECK(this->resizeBuffers(app, width, height));
        this->hasCapturedFrame_.clear();
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            this->hasCapturedFrame_.push_back(false);
        }
        return Ok();
    }

    Result<Unit, ErrorMessage> FrameRecorder::resizeBuffers(App& app, u32 width, u32 height)
    {
        this->width_ = width;
        this->height_ = height;
        
        this->buffers_.clear();
        this->buffers_.reserve(MAX_FRAMES_IN_FLIGHT);
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            auto bufferResult = MappedBuffer::fromBufferResult(
                FatBuffer::create(
                    app,
                    width * height * sizeof(u8) * 4,
                    vk::BufferUsageFlagBits::eTransferDst,
                    vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostCoherent
                )
            );
            CHECK_KEEP(bufferResult);
            this->buffers_.emplace_back(bufferResult.take());
        }
        return Ok();
    }

    Result<ScreenshotData, ErrorMessage> FrameRecorder::read(u32 frameIndex)
    {
        return ScreenshotData {
            .pixels = static_cast<const u8*>(this->buffers_[frameIndex].mapping.data()),
            .width = this->width_,
            .height = this->height_
        };
    }

    Result<Unit, ErrorMessage> FrameRecorder::save(u32 frameIndex)
    {
        assert(this->hasCapturedFrame_[frameIndex]);

        // read screenshot data
        auto readResult = this->read(frameIndex);
        CHECK_KEEP(readResult);
        ScreenshotData screenshot = readResult.take();

        // output it to the screenshot directory
        if (!std::filesystem::exists(this->outputDir_))
            std::filesystem::create_directory(this->outputDir_);
        std::filesystem::path path = this->outputDir_ / std::to_string(++this->screenshotNum_);
        path.replace_extension(".png");
        // std::ofstream out(path);
        // out.write(reinterpret_cast<const char*>(screenshot.pixels), screenshot.size());

        log({ .channel = "engine", .verbosity = 1}, "Saved screenshot {}.", path);
        this->hasCapturedFrame_[frameIndex] = false;
        return Ok();
    }
}