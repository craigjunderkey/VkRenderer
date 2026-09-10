#pragma once
#include "resource_type.h"

namespace VkRenderer::Resources::Types
{
    ResourceType textData();
    ResourceType jsonData();
    ResourceType imageData();
    ResourceType mesh();
    ResourceType material();
    ResourceType texture();
    ResourceType shader();
    ResourceType cubemap();
}