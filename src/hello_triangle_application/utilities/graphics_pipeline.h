#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createGraphicsPipeline(App& app);
}