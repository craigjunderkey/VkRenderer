#pragma once
#include "common.h"

namespace VkRenderer::RenderLoop { class GameLoop; }

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> drawFrame(App& app, RenderLoop::GameLoop& loop);
}