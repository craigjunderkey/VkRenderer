#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createSwapChain(App& app);
    Result<Unit, std::string> recreateSwapChain(App& app);
}