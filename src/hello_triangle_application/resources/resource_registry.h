#pragma once
#include "../utilities/common.h"
#include "resource_type.h"
#include "string_id_map.h"
#include "common_resource_processors.h"

#include "text_data.h"
#include <nlohmann/json.hpp>
#include "image_data.h"
#include "../geometry/mesh.h"
#include "../render_loop/material.h"
#include "../geometry/texture.h"
#include "shader.h"

#define REGISTER_RESOURCE_TYPE(type, resourceName)                                          \
private:                                                                                    \
    StringIdMap< type > resourceName##_ { TO_STRING_LITERAL(resourceName##_resource_map) }; \
public:                                                                                     \
    template <>                                                                             \
    StringIdMap< type >& getMap< type >()                                                   \
    {                                                                                       \
        return resourceName##_;                                                             \
    }                                                                                       \

namespace HelloTriangleApplication::Resources
{
    class ResourceRegistry
    {
    private:
        const std::vector<ResourceType>& types_;
        const StringIdMap<u32>& typeMap_;
        RuntimeProcessingData processingData_;
        bool hasShutdown_ = false;

    public:
        ResourceRegistry(const std::vector<ResourceType>& types, const StringIdMap<u32>& typeMap)
            : types_(types), typeMap_(typeMap), processingData_({}) {}
        
        RuntimeProcessingData& processingData() { return this->processingData_; }

        Result<Unit, std::string> startup()
        {
            NullRuntimeTasks defaultTasks;
            for (const ResourceType& type : this->types_)
            {
                RuntimeResourceTasks* tasks;
                if (type.runtime.tasks)
                    tasks = type.runtime.tasks.value();
                else
                    tasks = &defaultTasks;
                CHECK(tasks->onStartup(this->processingData_));
            }
            return Ok();
        }

        Result<Unit, std::string> shutdown()
        {
            if (this->hasShutdown_)
                panic("Called shutdown on ResourceRegistry but it's already been shut down.");
            NullRuntimeTasks defaultTasks;
            for (const ResourceType& type : this->types_)
            {
                RuntimeResourceTasks* tasks;
                if (type.runtime.tasks)
                    tasks = type.runtime.tasks.value();
                else
                    tasks = &defaultTasks;
                CHECK(tasks->onShutdown(this->processingData_));
            }
            this->hasShutdown_ = true;
            return Ok();
        }

        // ~ResourceRegistry()
        // {
        //     if (!this->hasShutdown_)
        //         panic("Destroyed ResourceRegistry without having first called shutdown.");
        // }

        // make a map for each resource type and associate it with a C++ type
        template <typename T>
        StringIdMap<T>& getMap()
        {
            panic("Type has no resource associated with it.");
        }
        REGISTER_RESOURCE_TYPE(TextData, text)
        REGISTER_RESOURCE_TYPE(nlohmann::json, json)
        REGISTER_RESOURCE_TYPE(ImageData, image)
        REGISTER_RESOURCE_TYPE(Geometry::Mesh, mesh)
        REGISTER_RESOURCE_TYPE(RenderLoop::Material, material)
        REGISTER_RESOURCE_TYPE(Geometry::Texture, texture)
        REGISTER_RESOURCE_TYPE(Shader, shader)

        template <typename T>
        Result<Unit, std::string> loadResources(StringId resourceName)
        {
            StringIdMap<T>& map = this->getMap<T>();
            const ResourceType& type = this->types_[this->typeMap_.get(resourceName)];

            // prepare loader pointer
            ResourceLoader* loader = type.runtime.loader;

            // load each asset in directory
            std::filesystem::path dir = RES_EXPORT_DIR / type.resourceDirectory;
            CHECK(loader->begin(this->processingData_));
            for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(dir))
            {
                std::filesystem::path path = file.path();
                T res;
                CHECK(loader->loadResource(path, &res, this->processingData_));
                CHECK(map.tryAdd(StringId(path.stem()), std::move(res)));
            }
            CHECK(loader->end(this->processingData_));

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
            CHECK(unloader->begin(this->processingData_));
            for (auto& pair : map)
            {
                CHECK(unloader->unloadResource(&pair.second, this->processingData_));
            }
            CHECK(unloader->end(this->processingData_));

            // clear the map / free the resources from memory
            map.clear();

            return Ok();
        }

        template <typename T>
        Result<Unit, std::string> postLoadInitialize(StringId resourceName)
        {
            StringIdMap<T>& map = this->getMap<T>();
            const ResourceType& type = this->types_[this->typeMap_.get(resourceName)];

            // prepare initializer pointer
            ResourcePostLoadInitializer* initializer;
            NullPostLoadInitializer defaultInitializer;
            if (type.runtime.postLoadInitializer)
                initializer = type.runtime.postLoadInitializer.value();
            else
                initializer = &defaultInitializer;
            
            // use initializer on each item in the resource map
            CHECK(initializer->begin(this->processingData_));
            for (auto& pair : map)
            {
                CHECK(initializer->postLoadInitializeResource(&pair.second, this->processingData_));
            }
            CHECK(initializer->end(this->processingData_));

            return Ok();
        }
    };
}