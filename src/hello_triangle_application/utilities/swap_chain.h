#pragma once
#include "common.h"

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createSwapChain(App& app);
    Result<Unit, ErrorMessage> recreateSwapChain(App& app);
}