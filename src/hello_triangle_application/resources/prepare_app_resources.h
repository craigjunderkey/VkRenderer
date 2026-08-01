#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> initializeResourceManager(App& app);
}