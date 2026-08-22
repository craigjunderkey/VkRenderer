#pragma once
#include "common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, ErrorMessage> createInstance(App& app);
}