#pragma once
#include "../utilities/common.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace HelloTriangleApplication::RenderLoop
{
    class ResourceReader
    {
    private:
        Option<std::filesystem::path> root_;
        Option<nlohmann::json> assetsJson_;

    public:
        Result<Unit, std::string> readAssetJson(std::filesystem::path path)
        {
            std::ifstream assetsFile(path);
            if (!assetsFile)
                return Err<std::string>("Could not open assets json (" + path.string() + ").");
            
            this->assetsJson_ = nlohmann::json::parse(assetsFile);

            #ifndef NDEBUG
                std::cout << "Loaded assets root (" << path << ")\n";
            #endif

            this->root_ = path.parent_path();

            return Ok();
        }

        Result<std::filesystem::path, std::string> getDirectory(std::string name)
        {
            assert(this->assetsJson_.has_value());
            const nlohmann::json& assetsJson = this->assetsJson_.value();

            if (!assetsJson.contains("directories"))
                return Err<std::string>("Assets json is missing a 'directories' key.");
            
            const nlohmann::json& directories = assetsJson["directories"];

            if (!directories.contains(name))
                return Err<std::string>("Directories section of assets json does not contain a '" + name + "' key.");
            
            std::string pathLabel = directories[name];
            std::filesystem::path relativePath = std::filesystem::path(std::move(pathLabel));

            assert(this->root_.has_value());
            const std::filesystem::path& root = this->root_.value();

            return root / relativePath;
        }

        Result<std::vector<std::filesystem::path>, std::string> readPaths(std::filesystem::path dir, std::string resourceKey)
        {
            assert(this->assetsJson_.has_value());
            const nlohmann::json& assets = this->assetsJson_.value();

            if (!assets.contains(resourceKey))
                return Err<std::string>("Assets json is missing a '" + resourceKey + "' key.");
            
            std::vector<std::filesystem::path> paths {};
            
            auto texList = assets[resourceKey];
            for (const auto& res : texList)
            {
                std::filesystem::path name = std::string(res);
                paths.emplace_back(dir / name);
            }

            return std::move(paths);
        }
        
        Result<std::vector<std::filesystem::path>, std::string> readResource(std::string resourceKey)
        {
            auto dirResult = getDirectory(resourceKey);
            CHECK_KEEP(dirResult);
            std::filesystem::path dir = dirResult.take();

            return readPaths(std::move(dir), std::move(resourceKey));
        }
    };
}