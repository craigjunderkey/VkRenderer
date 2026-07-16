#pragma once
#include "common.h"

namespace HelloTriangleApplication::RenderLoop { class GameLoop; }

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> drawFrame(App& app, RenderLoop::GameLoop& loop);
}