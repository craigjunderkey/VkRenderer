#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createCommandBuffer(App& app);
    void recordCommandBuffer(App& app, u32 imageIndex);
}