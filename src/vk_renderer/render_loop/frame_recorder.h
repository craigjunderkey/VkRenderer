#pragma once
#include "../utilities/common.h"
#include "../geometry/buffer.h"
#include "../resources/image_data.h"

namespace VkRenderer { class App; }

namespace VkRenderer::RenderLoop
{
    struct ScreenshotData
    {
        const u8* pixels;
        u32 width;
        u32 height;
        
        constexpr u32 size() const { return width * height * 3; }
    };
    
    class FrameRecorder
    {
    private:
        bool isFrameCaptureQueued_ = false;
        Geometry::MappedBuffer buffer_ = {};
        u32 width_;
        u32 height_;

    public:
        Result<Unit, ErrorMessage> captureFrame();
        Result<Unit, ErrorMessage> makeBuffer(App& app, u32 width, u32 height);
        Result<ScreenshotData, ErrorMessage> read();

        void enqueueFrameCapture() { this->isFrameCaptureQueued_ = true; }
        constexpr bool isFrameCaptureQueued() const { return this->isFrameCaptureQueued_; };
    };
}