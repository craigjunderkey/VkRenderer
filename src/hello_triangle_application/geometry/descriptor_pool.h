#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createDescriptorPool(App& app);
    Result<Unit, std::string> createDescriptorSets(App& app);
}