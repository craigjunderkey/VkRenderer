#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, ErrorMessage> createDescriptorPool(App& app);
    Result<Unit, ErrorMessage> createDescriptorSets(App& app);
}