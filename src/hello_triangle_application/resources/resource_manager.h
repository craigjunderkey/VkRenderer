#pragma once
#include "resource_type.h"
#include "asset_conditioning_pipeline.h"
#include "resource_registry.h"
#include "string_id_map.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Resources
{
    class NewResourceManager
    {
    private:
        std::vector<ResourceType> types_;
        StringIdMap<u32> typeMap_;
        AssetConditioningPipeline conditioningPipeline_;
        ResourceRegistry registry_;
    
    public:
        NewResourceManager(App& app)
            : types_({}), conditioningPipeline_(this->types_), registry_({ this->types_, this->typeMap_ })
        {
            this->registry_.processingData() = RuntimeProcessingData {
                .app = &app
            };
        }

        AssetConditioningPipeline& conditioningPipeline() { return this->conditioningPipeline_; }
        ResourceRegistry& registry() { return this->registry_; }

        void addResourceType(ResourceType type);
        bool exportExists();

        template <typename T>
        inline T& get(StringId id)
        {
            return this->registry_.get<T>(id);
        }
    };
}