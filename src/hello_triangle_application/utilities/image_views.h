#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createImageViews(App& app);
}