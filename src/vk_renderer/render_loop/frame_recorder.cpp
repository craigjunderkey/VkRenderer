#include "frame_recorder.h"

namespace VkRenderer::RenderLoop
{
    Result<Unit, ErrorMessage> FrameRecorder::captureFrame()
    {
        assert(this->isFrameCaptureQueued_);
        log({}, "snap!");
        this->isFrameCaptureQueued_ = false;
        return Ok();
    }
}