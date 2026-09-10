#pragma once
#include "../utilities/common.h"

namespace VkRenderer { class App; }

namespace VkRenderer::Geometry
{
    Result<Unit, ErrorMessage> createDescriptorPool(App& app);
    Result<Unit, ErrorMessage> createDescriptorSets(App& app);
}