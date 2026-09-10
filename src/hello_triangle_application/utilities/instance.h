#pragma once
#include "common.h"

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createInstance(App& app);
}