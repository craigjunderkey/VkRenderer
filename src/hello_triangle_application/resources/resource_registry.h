#pragma once
#include "../utilities/common.h"
#include "resource_type.h"
#include "string_id_map.h"
#include "common_resource_processors.h"

#include "text_data.h"
#include <nlohmann/json.hpp>
#include "image_data.h"

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
        RuntimeProcessingData processingData_;

    public:
        ResourceRegistry(const std::vector<ResourceType>& types, const StringIdMap<u32>& typeMap)
            : types_(types), typeMap_(typeMap), processingData_({}) {}

        // make a map for each resource type and associate it with a C++ type
        template <typename T>
        StringIdMap<T>& getMap()
        {
            panic("Type has no resource associated with it.");
        }
        REGISTER_RESOURCE_TYPE(TextData, text)
        REGISTER_RESOURCE_TYPE(nlohmann::json, json)
        REGISTER_RESOURCE_TYPE(ImageData, image)

        template <typename T>
        Result<Unit, std::string> loadResources(StringId resourceName)
        {
            StringIdMap<T>& map = this->getMap<T>();
            const ResourceType& type = this->types_[this->typeMap_.get(resourceName)];

            // prepare loader pointer
            ResourceLoader* loader = type.runtime.loader;

            // load each asset in directory
            std::filesystem::path dir = RES_EXPORT_DIR / type.resourceDirectory;
            for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(dir))
            {
                std::filesystem::path path = file.path();
                T res;
                CHECK(loader->loadResource(path, &res, this->processingData_));
                CHECK(map.tryAdd(StringId(path.stem()), std::move(res)));
            }

            return Ok();
        }

        template <typename T>
        T& get(StringId id)
        {
            return this->getMap<T>().get(id);
        }

        template <typename T>
        Result<Unit, std::string> unloadResources(StringId resourceName)
        {
            StringIdMap<T>& map = this->getMap<T>();
            const ResourceType& type = this->types_[this->typeMap_.get(resourceName)];

            // prepare unloader pointer
            ResourceUnloader* unloader;
            NullUnloader defaultUnloader;
            if (type.runtime.unloader)
                unloader = type.runtime.unloader.value();
            else
                unloader = &defaultUnloader;

            // unload each item in the resource map
            for (auto& pair : map)
            {
                CHECK(unloader->unloadResource(&pair.second, this->processingData_));
            }

            // clear the map / free the resources from memory
            map.clear();

            return Ok();
        }
    };
}