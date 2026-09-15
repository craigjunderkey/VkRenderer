#pragma once
#include "../utilities/common.h"
#include "../geometry/buffer.h"
#include <filesystem>

namespace VkRenderer { class App; }

namespace VkRenderer::RenderLoop
{
    struct ScreenshotData
    {
        const u8* pixels;
        u32 width;
        u32 height;
        
        constexpr u32 size() const { return width * height * 4; }
    };
    
    class FrameRecorder
    {
    private:
        bool isFrameCaptureQueued_ = false;
        std::vector<bool> hasCapturedFrame_ = {};
        std::vector<Geometry::MappedBuffer> buffers_ = {};
        u32 width_;
        u32 height_;

        // testing
        std::filesystem::path outputDir_ = "screenshots";
        u32 screenshotNum_ = 0;

    public:
        Result<Unit, ErrorMessage> initialize(App& app, u32 width, u32 height);
        Result<Unit, ErrorMessage> captureFrame(App& app, vk::raii::CommandBuffer& cmdBuffer, vk::Image swapchainImage);
        Result<Unit, ErrorMessage> resizeBuffers(App& app, u32 width, u32 height);
        Result<ScreenshotData, ErrorMessage> read(u32 frameIndex);
        Result<Unit, ErrorMessage> save(u32 frameIndex);

        void enqueueFrameCapture() { this->isFrameCaptureQueued_ = true; }
        constexpr bool isFrameCaptureQueued() const { return this->isFrameCaptureQueued_; }
        constexpr bool hasCapturedFrame(u32 frameIndex) const { return this->hasCapturedFrame_[frameIndex]; }
    };
}