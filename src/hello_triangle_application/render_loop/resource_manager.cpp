#include "../hello_triangle_application.h"
#include "resource_manager.h"
#include "../geometry/model_loader.h"
#include "resource_reader.h"

using namespace HelloTriangleApplication::Geometry;

namespace HelloTriangleApplication::RenderLoop
{
    Result<Unit, std::string> ResourceManager::loadTextures(App& app, std::vector<std::filesystem::path> paths)
    {
        for (const auto& path : paths)
        {
            // load texture
            std::string pathString = path.string();
            auto loadResult = Texture::loadFromFile(app, pathString);
            CHECK_KEEP(loadResult);
            Texture texture = loadResult.take();
            auto samplerResult = texture.makeSampler(app);
            CHECK(samplerResult);

            // add it to the map
            StringId id = pathToStringId(path);
            
            #ifndef NDEBUG
                std::cout << "Loaded texture " << id.string() << " (" << pathString << ")\n";
            #endif

            auto pair = std::tuple<StringId, Texture>(std::move(id), std::move(texture));
            auto insertResult = this->textures.insert(std::move(pair));
            
            if (!insertResult.second)
                return Err<std::string>("Tried to load " + pathString + " multiple times.");
        }

        return Ok();
    }

    Result<Unit, std::string> ResourceManager::loadMaterials(App& app, std::vector<MaterialResourceData> data)
    {
        for (const MaterialResourceData& matData : data)
        {
            // load material
            Material mat {
                .texture = this->texture(matData.texture),
                .texOffset = matData.texOffset,
                .texScale = matData.texScale,
                .ambient = matData.ambient,
                .diffuse = matData.diffuse,
                .specular = matData.specular,
                .shininess = matData.shininess,
                .isUnlit = matData.isUnlit
            };

            // add it to the map
            StringId id = std::move(matData.name);

            #ifndef NDEBUG
                std::cout << "Loaded material " << id.string() << "\n";
            #endif

            auto pair = std::tuple<StringId, Material>(std::move(id), std::move(mat));
            auto insertResult = this->materials.insert(std::move(pair));

            if (!insertResult.second)
                return Err<std::string>("Tried to load material " + std::string(id) + " multiple times.");
        }

        return Ok();
    }

    Result<Unit, std::string> ResourceManager::loadMeshes(App& app, std::vector<std::filesystem::path> paths)
    {
        for (const auto& path : paths)
        {
            //load mesh
            std::string pathString = path.string();
            auto dataResult = loadMeshData<u32>(pathString);
            CHECK_KEEP(dataResult);
            Mesh<u32> mesh = Mesh<u32>::fromData(std::move(dataResult.take()));
            CHECK(mesh.makeBuffers(app));

            // add it to the map
            StringId id = pathToStringId(path);

            #ifndef NDEBUG
                std::cout << "Loaded mesh " << id.string() << " (" << pathString << ")\n";
            #endif

            auto pair = std::tuple<StringId, Mesh<u32>>(std::move(id), std::move(mesh));
            auto insertResult = this->meshes.insert(std::move(pair));

            if (!insertResult.second)
                return Err<std::string>("Tried to load " + pathString + " multiple times.");
        }

        return Ok();
    }

    Result<Unit, std::string> ResourceManager::loadResources(App& app, std::filesystem::path assetsPath)
    {
        // load assets json
        ResourceReader reader;
        CHECK(reader.readAssetJson(std::move(assetsPath)));

        // load textures
        {
            auto texPathsResult = reader.readResourcePaths("textures");
            CHECK_KEEP(texPathsResult);
            std::vector<std::filesystem::path> texPaths = texPathsResult.take();
            auto texLoadResult = this->loadTextures(app, std::move(texPaths));
            CHECK(texLoadResult);
        }

        // load meshes
        {
            auto meshPathsResult = reader.readResourcePaths("meshes");
            CHECK_KEEP(meshPathsResult);
            std::vector<std::filesystem::path> meshPaths = meshPathsResult.take();
            auto meshLoadResult = this->loadMeshes(app, std::move(meshPaths));
            CHECK(meshLoadResult);
        }

        // load materials
        {
            auto matDataResult = reader.readMaterialData();
            CHECK_KEEP(matDataResult);
            std::vector<MaterialResourceData> matData = matDataResult.take();
            auto matLoadResult = this->loadMaterials(app, std::move(matData));
            CHECK(matLoadResult);
        }

        return Ok();
    }
}