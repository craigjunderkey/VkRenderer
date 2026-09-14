#pragma once
#include "../utilities/common.h"

namespace VkRenderer::RenderLoop
{
    class FrameRecorder
    {
    private:
        bool isFrameCaptureQueued_ = false;

    public:
        Result<Unit, ErrorMessage> captureFrame();

        void enqueueFrameCapture() { this->isFrameCaptureQueued_ = true; }
        constexpr bool isFrameCaptureQueued() const { return this->isFrameCaptureQueued_; };
    };
}