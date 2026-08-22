#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, ErrorMessage> createDepthTexture(App& app);
}