#include "../../hello_triangle_application.h"
#include "material_processing.h"
#include "../json_helper.h"
#include "../../render_loop/material.h"
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
        App& app = *processingData.app;
        BindlessManager& bindless = app.resources.bindless();
        
        // get ptr vec
        std::vector<StringId> matIdVec = app.resources.registry().getMap<RenderLoop::Material>().makeIdVec();
        std::sort(matIdVec.begin(), matIdVec.end());
        std::vector<const RenderLoop::Material*> matVec = app.resources.registry().getMap<RenderLoop::Material>().makePtrVec(matIdVec);

        // create material uniform data
        std::vector<RenderLoop::MaterialUniformData> matData;
        u32 numMaterials = bindless.getBindlessResource("material").virtualCount;
        matData.reserve(numMaterials);
        for (u32 i = 0; i < numMaterials; i++)
        {
            const RenderLoop::Material* mat = matVec[i];
            matData.push_back(RenderLoop::MaterialUniformData {
                .diffMap = app.resources.bindless().getNullableIndex("texture", mat->diffuseTexture).unwrap(),
                .specMap = app.resources.bindless().getNullableIndex("texture", mat->specularTexture).unwrap(),
                .samplerId = static_cast<u32>(mat->sampler),
                .shininess = mat->shininess,
                .isUnlit = mat->isUnlit,
                .texOffset = mat->texOffset,
                .texScale = mat->texScale,
                .diffColor = mat->diffuseColor,
                .specColor = mat->specularColor
            });
        }

        // buffer the material uniform data
        Geometry::MappedBuffer& matBuffer = bindless.getBuffer("materials");
        memcpy(matBuffer.data, matData.data(), matBuffer.buffer.size);

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