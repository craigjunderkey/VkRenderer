#include "../hello_triangle_application.h"
#include "resource_manager.h"

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
                std::cout << "Loaded texture " << id.string() << " (" << pathString << ").\n";
            #endif

            auto pair = std::tuple<StringId, Texture>(std::move(id), std::move(texture));
            auto insertResult = this->textures.insert(std::move(pair));
            
            if (!insertResult.second)
                return Err<std::string>("Tried to load " + pathString + " multiple times.");
        }

        return Ok();
    }

    Result<Unit, std::string> ResourceManager::loadMaterials(App& app, std::vector<std::filesystem::path> paths)
    {
        return Ok();
    }

    Result<Unit, std::string> ResourceManager::loadMeshes(App& app, std::vector<std::filesystem::path> paths)
    {
        return Ok();
    }
}