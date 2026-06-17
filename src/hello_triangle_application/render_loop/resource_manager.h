#pragma once
#include "../utilities/common.h"
#include "material.h"
#include "../geometry/texture.h"
#include "../geometry/mesh.h"

#include <unordered_map>
#include <filesystem>

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    inline StringId pathToStringId(std::filesystem::path path) { return StringId(path.stem()); }

    class ResourceManager
    {
    private:
        std::unordered_map<StringId, Geometry::Texture> textures;
        std::unordered_map<StringId, Material> materials;
        std::unordered_map<StringId, Geometry::Mesh<u32>> meshes;

    public:
        inline const Geometry::Texture& texture(StringId id) const { return this->textures.find(id)->second; }
        inline const Material& material(StringId id) const { return this->materials.find(id)->second; }
        inline const Geometry::Mesh<u32>& mesh(StringId id) const { return this->meshes.find(id)->second; }

        Result<Unit, std::string> loadTextures(App& app, std::vector<std::filesystem::path> paths);
        Result<Unit, std::string> loadMaterials(App& app, std::vector<std::filesystem::path> paths);
        Result<Unit, std::string> loadMeshes(App& app, std::vector<std::filesystem::path> paths);
    };
}