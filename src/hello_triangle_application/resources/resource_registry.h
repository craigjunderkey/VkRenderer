#pragma once
#include "../utilities/common.h"
#include "resource_type.h"
#include "string_id_map.h"

#include "resource_text_data.h"
#include <nlohmann/json.hpp>

#define REGISTER_RESOURCE_TYPE(type, resourceName) \
private:                                        \
    StringIdMap< type > resourceName##_;        \
public:                                         \
    template <>                                 \
    StringIdMap< type >& getMap< type >()       \
    {                                           \
        return resourceName##_;                 \
    }                                           \

namespace HelloTriangleApplication::Resources
{
    class ResourceRegistry
    {
    private:
        const std::vector<ResourceType>& types_;
        const StringIdMap<u32>& typeMap_;

    public:
        ResourceRegistry(const std::vector<ResourceType>& types, const StringIdMap<u32>& typeMap) : types_(types), typeMap_(typeMap) {}

        // make a map for each resource type and associate it with a C++ type
        template <typename T>
        StringIdMap<T>& getMap()
        {
            panic("Type has no resource associated with it.");
        }
        REGISTER_RESOURCE_TYPE(TextData, text)
        REGISTER_RESOURCE_TYPE(nlohmann::json, json)

        template <typename T>
        Result<Unit, std::string> loadResources(StringId resourceName)
        {
            StringIdMap<T>& map = this->getMap<T>();
            std::cout << "Found a map for " << std::string(resourceName) << std::endl;
            return Ok();
        }
    };
}