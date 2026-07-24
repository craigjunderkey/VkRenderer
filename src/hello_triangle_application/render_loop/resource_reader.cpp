#include "resource_reader.h"
#include "json_helper.h"

namespace HelloTriangleApplication::RenderLoop
{
    Result<Unit, std::string> ResourceReader::readAssetJson(std::filesystem::path path)
    {
        std::ifstream assetsFile(path);
        if (!assetsFile)
            return Err<std::string>("Could not open assets json (" + path.string() + ").");
        
        this->assetsJson_ = nlohmann::json::parse(assetsFile);

        #ifndef NDEBUG
            std::cout << "Loaded assets root (" << path.string() << ")\n";
        #endif

        this->root_ = path.parent_path();

        return Ok();
    }

    Result<std::filesystem::path, std::string> ResourceReader::getDirectory(std::string name)
    {
        assert(this->assetsJson_.has_value());
        const nlohmann::json& assetsJson = this->assetsJson_.value();

        CHECK_KEY(assetsJson, "directories");
        const nlohmann::json& directories = assetsJson["directories"];

        CHECK_KEY(directories, name);
        std::string pathLabel = directories[name];
        std::filesystem::path relativePath = std::filesystem::path(std::move(pathLabel));

        assert(this->root_.has_value());
        const std::filesystem::path& root = this->root_.value();

        return root / relativePath;
    }

    Result<std::vector<std::filesystem::path>, std::string> ResourceReader::readPaths(
        std::filesystem::path dir, std::string resourceKey)
    {
        assert(this->assetsJson_.has_value());
        const nlohmann::json& assets = this->assetsJson_.value();

        CHECK_KEY(assets, resourceKey);
        auto resList = assets[resourceKey];
        
        std::vector<std::filesystem::path> paths {};
        
        for (const auto& res : resList)
        {
            std::filesystem::path name = std::string(res);
            paths.emplace_back(dir / name);
        }

        return std::move(paths);
    }
    
    Result<std::vector<std::filesystem::path>, std::string> ResourceReader::readResourcePaths(std::string resourceKey)
    {
        auto dirResult = getDirectory(resourceKey);
        CHECK_KEEP(dirResult);
        std::filesystem::path dir = dirResult.take();

        return readPaths(std::move(dir), std::move(resourceKey));
    }

    Result<Material, std::string> ResourceReader::readMaterialData(
        const std::string& name, const nlohmann::json& data)
    {
        Material mat { .name = StringId(name) };
        JsonHelper json(data);
        
        json.trySetProperty("texture", mat.diffuseTexture);
        json.trySetProperty("diffuseTexture", mat.diffuseTexture);
        json.trySetProperty("specularTexture", mat.specularTexture);
        json.trySetProperty("texOffset", mat.texOffset);
        json.trySetProperty("texScale", mat.texScale);
        json.trySetProperty("diffuseColor", mat.diffuseColor);
        json.trySetProperty("specularColor", mat.specularColor);
        json.trySetProperty("shininess", mat.shininess);
        json.trySetProperty("isUnlit", mat.isUnlit);

        if (data.contains("sampler")) mat.sampler = SamplerManager::samplerNameFromString(std::string(data["sampler"]).data());

        return std::move(mat);
    }
    
    Result<std::vector<Material>, std::string> ResourceReader::readMaterialData()
    {
        assert(this->assetsJson_.has_value());
        const nlohmann::json& assets = this->assetsJson_.value();

        CHECK_KEY(assets, "materials");
        auto matList = assets["materials"].items();

        std::vector<Material> data {};

        for (const auto& mat : matList)
        {
            auto matReadResult = readMaterialData(mat.key(), mat.value());
            CHECK_KEEP(matReadResult);
            data.emplace_back(matReadResult.take());
        }

        return std::move(data);
    }
};