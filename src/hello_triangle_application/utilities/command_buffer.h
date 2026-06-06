#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createCommandBuffers(App& app);
    void recordCommandBuffer(App& app, u32 imageIndex);
}