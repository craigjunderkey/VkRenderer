#include "frame_recorder.h"

using namespace VkRenderer::Geometry;

namespace VkRenderer::RenderLoop
{
    Result<Unit, ErrorMessage> FrameRecorder::captureFrame()
    {
        assert(this->isFrameCaptureQueued_);
        log({}, "snap!");
        this->isFrameCaptureQueued_ = false;
        return Ok();
    }

    Result<Unit, ErrorMessage> FrameRecorder::makeBuffer(App& app, u32 width, u32 height)
    {
        this->width_ = width;
        this->height_ = height;
        
        auto bufferResult = MappedBuffer::fromBufferResult(
            FatBuffer::create(
                app,
                width * height * sizeof(u8) * 4,
                vk::BufferUsageFlagBits::eTransferDst,
                vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );
        CHECK_KEEP(bufferResult);
        this->buffer_ = bufferResult.take();
        return Ok();
    }

    Result<ScreenshotData, ErrorMessage> FrameRecorder::read()
    {
        return ScreenshotData {
            .pixels = static_cast<const u8*>(this->buffer_.mapping.data()),
            .width = this->width_,
            .height = this->height_
        };
    }
}