#pragma once
#include "../utilities/common.h"
#include "material.h"
#include "../geometry/texture.h"
#include "material_manager.h"
#include "sampler_manager.h"
#include "../geometry/mesh.h"

#include <unordered_map>
#include <filesystem>

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    inline StringId pathToStringId(std::filesystem::path path) { return StringId(path.stem()); }

    struct RawLoadedResources
    {
        std::vector<std::filesystem::path> texPaths;
        std::vector<std::filesystem::path> meshPaths;
        std::vector<Material> matData;
    };

    class ResourceManager
    {
    private:
        Option<RawLoadedResources> loaded_;
        std::unordered_map<StringId, Geometry::Texture> textures;
        std::unordered_map<StringId, Material> materials;
        std::unordered_map<StringId, Geometry::Mesh> meshes;
        MaterialManager matManager_;
        SamplerManager samplerManager_;

    public:
        inline const Geometry::Texture& texture(StringId id) const
        {
            auto result = this->textures.find(id);
            assert(result != this->textures.end());
            return result->second;
        }
        
        inline const Material& material(StringId id) const
        {
            auto result = this->materials.find(id);
            assert(result != this->materials.end());
            return result->second;
        }
        
        inline const Geometry::Mesh& mesh(StringId id) const
        {
            auto result = this->meshes.find(id);
            assert(result != this->meshes.end());
            return result->second;
        }

        inline const MaterialManager& materialManager() const { return this->matManager_; }
        inline const SamplerManager& samplerManager() const { return this->samplerManager_; }

        Result<Unit, std::string> loadTextures(App& app, std::vector<std::filesystem::path> paths);
        Result<Unit, std::string> loadMaterials(App& app, std::vector<Material> data);
        Result<Unit, std::string> loadMeshes(App& app, std::vector<std::filesystem::path> paths);
        Result<Unit, std::string> prepareResources(App& app, std::filesystem::path assetsJsonPath);
        Result<Unit, std::string> loadResources(App& app);
    };
}