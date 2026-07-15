#include "../hello_triangle_application.h"
#include "resource_manager.h"
#include "../geometry/model_loader.h"
#include "resource_reader.h"
#include "../geometry/texture.h"

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
            texture.sampler = SamplerName::Default;

            // add it to the map
            StringId id = pathToStringId(path);
            texture.name = id;
            
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
                .name = std::move(matData.name),
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

        this->matManager_.writeMaterialData(app, this->textures, this->materials);

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
            mesh.name = id;

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

    Result<Unit, std::string> ResourceManager::prepareResources(App& app, std::filesystem::path assetsPath)
    {
        // load assets json
        ResourceReader reader;
        CHECK(reader.readAssetJson(std::move(assetsPath)));

        RawLoadedResources loaded {};

        // load textures
        auto texPathsResult = reader.readResourcePaths("textures");
        CHECK_KEEP(texPathsResult);
        loaded.texPaths = texPathsResult.take();

        // load meshes
        auto meshPathsResult = reader.readResourcePaths("meshes");
        CHECK_KEEP(meshPathsResult);
        loaded.meshPaths = meshPathsResult.take();

        // load materials
        auto matDataResult = reader.readMaterialData();
        CHECK_KEEP(matDataResult);
        loaded.matData = matDataResult.take();

        // create material manager
        TRY_ASSIGN(
            this->matManager_,
            MaterialManager::create(
                app, loaded.matData.size(), loaded.texPaths.size(), this->samplerManager_.numSamplers()
            )
        );

        // store what we loaded
        this->loaded_ = std::move(loaded);

        return Ok();
    }

    Result<Unit, std::string> ResourceManager::loadResources(App& app)
    {
        assert(this->loaded_.has_value());

        RawLoadedResources loaded = std::move(this->loaded_.value());

        // import samplers
        auto samplerLoadResult = this->samplerManager_.loadSamplers(app);
        CHECK(samplerLoadResult);

        // import textures
        auto texLoadResult = this->loadTextures(app, std::move(loaded.texPaths));
        CHECK(texLoadResult);

        // import meshes
        auto meshLoadResult = this->loadMeshes(app, std::move(loaded.meshPaths));
        CHECK(meshLoadResult);

        // import materials
        auto matLoadResult = this->loadMaterials(app, std::move(loaded.matData));
        CHECK(matLoadResult);

        return Ok();
    }
}