#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, ErrorMessage> createSurface(App& app);
}