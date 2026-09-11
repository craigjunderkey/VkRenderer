#pragma once
#include "../utilities/common.h"
#include "resource_type.h"

namespace VkRenderer::Resources
{
    class AssetConditioningPipeline
    {
    private:
        const std::vector<ResourceType>& types_;
    
    public:
        AssetConditioningPipeline(const std::vector<ResourceType>& types) : types_(types) {}
        Result<std::vector<std::filesystem::path>, ErrorMessage> findAssets(const ResourceType& resourceType);
        Result<Unit, ErrorMessage> prepareExportDirectory();
        Result<Unit, ErrorMessage> buildAssets();
    };
}