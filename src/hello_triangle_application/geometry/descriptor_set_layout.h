#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createDescriptorSetLayout(App& app);
}