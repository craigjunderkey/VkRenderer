#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createBuffers(App& app);
}