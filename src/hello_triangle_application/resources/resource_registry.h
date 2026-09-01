#pragma once
#include "../utilities/common.h"
#include "resource_type.h"
#include "../../utility/string_id_map.h"
#include "processors/common_resource_processors.h"

#include "text_data.h"
#include <nlohmann/json.hpp>
#include "image_data.h"
#include "../geometry/mesh.h"
#include "../render_loop/material.h"
#include "../geometry/texture.h"
#include "shader.h"
#include "../render_loop/cubemap.h"

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

        Result<Unit, ErrorMessage> startup()
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

        Result<Unit, ErrorMessage> shutdown()
        {
            if (this->hasShutdown_)
            {
                warn({ .channel = "resources" }, "Called shutdown on ResourceRegistry but it's already shutdown.");
                return Ok();
            }

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

        // make a map for each resource type and associate it with a C++ type
        template <typename T>
        StringIdMap<T>& getMap()
        {   
            err({ .channel = "resource" }, "Type has no resource associated with it.");
            panic();
        }

        REGISTER_RESOURCE_TYPE(TextData, text)
        REGISTER_RESOURCE_TYPE(nlohmann::json, json)
        REGISTER_RESOURCE_TYPE(ImageData, image)
        REGISTER_RESOURCE_TYPE(Geometry::Mesh, mesh)
        REGISTER_RESOURCE_TYPE(RenderLoop::Material, material)
        REGISTER_RESOURCE_TYPE(Geometry::Texture, texture)
        REGISTER_RESOURCE_TYPE(Shader, shader)
        REGISTER_RESOURCE_TYPE(RenderLoop::Cubemap, cubemap)

        template <typename T>
        Result<Unit, ErrorMessage> loadResources(StringId resourceName)
        {
            log({ .channel = "resources", .verbosity = 2}, "Loading resource {}.", resourceName);
            
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
        Result<Unit, ErrorMessage> unloadResources(StringId resourceName)
        {
            log({ .channel = "resources", .verbosity = 2}, "Unloading resource {}.", resourceName);

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
        Result<Unit, ErrorMessage> postLoadInitialize(StringId resourceName)
        {
            log({ .channel = "resources", .verbosity = 2}, "Post-load initializing resource {}.", resourceName);

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