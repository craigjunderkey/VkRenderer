#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, ErrorMessage> createSwapChain(App& app);
    Result<Unit, ErrorMessage> recreateSwapChain(App& app);
}