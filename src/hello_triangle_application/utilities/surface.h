#pragma once
#include "common.h"

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createSurface(App& app);
}