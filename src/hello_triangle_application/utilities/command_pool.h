#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, ErrorMessage> createCommandPools(App& app);
}