#pragma once
#include "../utilities/common.h"
#include "resource_type.h"
#include <vector>
#include <string>

namespace VkRenderer::Resources
{
    struct TextData
    {
        std::vector<std::string> lines;
    };
}