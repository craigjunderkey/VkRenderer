#pragma once
#include "../utilities/common.h"
#include "sampler_manager.h"
#include "material.h"

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
        Result<Unit, std::string> readAssetJson(std::filesystem::path path);
        Result<std::filesystem::path, std::string> getDirectory(std::string name);
        Result<std::vector<std::filesystem::path>, std::string> readPaths(std::filesystem::path dir, std::string resourceKey);
        Result<std::vector<std::filesystem::path>, std::string> readResourcePaths(std::string resourceKey);
        Result<Material, std::string> readMaterialData(const std::string& name, const nlohmann::json& data);
        Result<std::vector<Material>, std::string> readMaterialData();
    };
}