#pragma once
#include "resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class ImageDataUnloader : public ResourceUnloader
    {
    public:
        Result<Unit, std::string> unloadResource(std::any resource, RuntimeProcessingData& processingData) override;
    };
}