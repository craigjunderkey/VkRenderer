#include "../hello_triangle_application.h"
#include "material_processing.h"
#include "../render_loop/json_helper.h"
#include "../render_loop/material.h"
#include <iostream>
#include <fstream>

using namespace HelloTriangleApplication::RenderLoop;

namespace HelloTriangleApplication::Resources
{
    Material loadMaterialJson(const nlohmann::json& json)
    {
        JsonHelper helper(json);
        Material mat;

        helper.trySetProperty("texture", mat.diffuseTexture);
        helper.trySetProperty("diffuseTexture", mat.diffuseTexture);
        helper.trySetProperty("specularTexture", mat.specularTexture);
        helper.trySetProperty("texOffset", mat.texOffset);
        helper.trySetProperty("texScale", mat.texScale);
        helper.trySetProperty("diffuseColor", mat.diffuseColor);
        helper.trySetProperty("specularColor", mat.specularColor);
        helper.trySetProperty("shininess", mat.shininess);
        helper.trySetProperty("isUnlit", mat.isUnlit);

        if (json.contains("sampler"))
            mat.sampler = SamplerManager::samplerNameFromString(std::string(json["sampler"]).data());

        return std::move(mat);
    }
    
    Result<Unit, std::string> MaterialExporter::exportResource(const std::filesystem::path& path,
        OfflineProcessingData& processingData)
    {
        auto json = JsonHelper::load(path);
        
        std::filesystem::path dir = getExportDirectory("materials");
        auto iter = json.begin();
        while (iter != json.end())
        {
            // load material data
            Material mat = loadMaterialJson(iter.value());
            std::string matName = iter.key();
            mat.name = StringId(matName);

            // save it into a file
            std::filesystem::path outPath = dir / matName;
            outPath.replace_extension(".mat");
            std::ofstream out(outPath, std::ios::binary);
            out.write((const char*)&mat, sizeof(Material));

            iter++;
        }
        
        return Ok();
    }

    Result<Unit, std::string> MaterialLoader::loadResource(const std::filesystem::path& path,
        std::any resource, RuntimeProcessingData& processingData)
    {
        Material* mat = std::any_cast<Material*>(resource);
        std::ifstream in(path, std::ios::binary);
        in.read(reinterpret_cast<char*>(mat), sizeof(Material));
        in.ignore();
        if (!in.eof())
            return Err<std::string>("Read all material info in " + path.string() + " but didn't reach end-of-file.\n");
        return Ok();
    }

    Result<Unit, std::string> MaterialInitializer::begin(RuntimeProcessingData& processingData)
    {
        // write material data
        App& app = *processingData.app;
        auto matWriteResult = app.gpuMaterials.writeMaterialData(
            app,
            app.resources.registry().getMap<Texture>(),
            app.resources.registry().getMap<Material>()
        );
        CHECK(matWriteResult);

        return Ok();
    }

    Result<Unit, std::string> MaterialInitializer::end(RuntimeProcessingData& processingData)
    {
        return Ok();
    }

    Result<Unit, std::string> MaterialInitializer::postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData)
    {
        return Ok();
    }
}