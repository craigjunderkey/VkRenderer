#pragma once
#include "common.h"

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createCommandPools(App& app);
}