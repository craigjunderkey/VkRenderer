#pragma once
#include "resource_type.h"
#include "asset_conditioning_pipeline.h"
#include "resource_registry.h"
#include "bindless_manager.h"
#include "../../utility/string_id_map.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Resources
{
    class ResourceManager
    {
    private:
        std::vector<ResourceType> types_;
        StringIdMap<u32> typeMap_ { "resource_type_to_index_map" };
        AssetConditioningPipeline conditioningPipeline_;
        ResourceRegistry registry_;
        BindlessManager bindless_;
    
    public:
        ResourceManager(App& app);

        AssetConditioningPipeline& conditioningPipeline() { return this->conditioningPipeline_; }
        ResourceRegistry& registry() { return this->registry_; }
        BindlessManager& bindless() { return this->bindless_; }

        void addResourceType(ResourceType type);
        bool exportExists();

        template <typename T>
        inline T& get(StringId id)
        {
            return this->registry_.get<T>(id);
        }
    };
}