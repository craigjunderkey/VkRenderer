#pragma once
#include "../utilities/common.h"
#include "resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class AssetConditioningPipeline
    {
    private:
        const std::vector<ResourceType>& types_;
    
    public:
        AssetConditioningPipeline(const std::vector<ResourceType>& types) : types_(types) {}
        Result<std::vector<std::filesystem::path>, std::string> findAssets(const ResourceType& resourceType);
        Result<Unit, std::string> prepareExportDirectory();
        Result<Unit, std::string> buildAssets();
    };
}