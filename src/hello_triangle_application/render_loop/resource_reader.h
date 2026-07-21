#pragma once
#include "../utilities/common.h"
#include "resource_data.h"
#include "sampler_manager.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#define CHECK_KEY(json, key) if (!json.contains(key)) return Err<std::string>("Missing '" + std::string(key) + "' key.")

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
                std::cout << "Loaded assets root (" << path.string() << ")\n";
            #endif

            this->root_ = path.parent_path();

            return Ok();
        }

        Result<std::filesystem::path, std::string> getDirectory(std::string name)
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

        Result<std::vector<std::filesystem::path>, std::string> readPaths(std::filesystem::path dir, std::string resourceKey)
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
        
        Result<std::vector<std::filesystem::path>, std::string> readResourcePaths(std::string resourceKey)
        {
            auto dirResult = getDirectory(resourceKey);
            CHECK_KEEP(dirResult);
            std::filesystem::path dir = dirResult.take();

            return readPaths(std::move(dir), std::move(resourceKey));
        }

        Result<MaterialResourceData, std::string> readMaterialData(const std::string& name, const nlohmann::json& data)
        {
            MaterialResourceData mat { .name = StringId(name) };
            
            if (data.contains("texture")) mat.texture = StringId(data["texture"]);
            if (data.contains("sampler")) mat.sampler = SamplerManager::samplerNameFromString(std::string(data["sampler"]).data());
            if (data.contains("texOffset")) mat.texOffset = vec2(data["texOffset"][0], data["texOffset"][1]);
            if (data.contains("texScale")) mat.texScale = vec2(data["texScale"][0], data["texScale"][1]);
            if (data.contains("ambient")) mat.ambient = vec3(data["ambient"][0], data["ambient"][1], data["ambient"][2]);
            if (data.contains("diffuse")) mat.diffuse = vec3(data["diffuse"][0], data["diffuse"][1], data["diffuse"][2]);
            if (data.contains("specular")) mat.specular = vec3(data["specular"][0], data["specular"][1], data["specular"][2]);
            if (data.contains("shininess")) mat.shininess = data["shininess"];
            if (data.contains("isUnlit")) mat.isUnlit = data["isUnlit"];

            return std::move(mat);
        }
        
        Result<std::vector<MaterialResourceData>, std::string> readMaterialData()
        {
            assert(this->assetsJson_.has_value());
            const nlohmann::json& assets = this->assetsJson_.value();

            CHECK_KEY(assets, "materials");
            auto matList = assets["materials"].items();

            std::vector<MaterialResourceData> data {};

            for (const auto& mat : matList)
            {
                auto matReadResult = readMaterialData(mat.key(), mat.value());
                CHECK_KEEP(matReadResult);
                data.emplace_back(matReadResult.take());
            }

            return std::move(data);
        }
    };
}