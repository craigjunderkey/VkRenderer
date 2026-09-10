#pragma once
#include "../utilities/common.h"

namespace VkRenderer { class App; }

namespace VkRenderer::Geometry
{
    Result<Unit, ErrorMessage> createColorTexture(App& app);
}