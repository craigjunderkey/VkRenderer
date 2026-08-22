#include "../../hello_triangle_application.h"
#include "cubemap_processing.h"
#include "../json_helper.h"
#include "../../render_loop/cubemap.h"
#include "../../geometry/texture.h"
#include <iostream>
#include <fstream>

using HelloTriangleApplication::RenderLoop::Cubemap;
using HelloTriangleApplication::RenderLoop::CubeSide;
using HelloTriangleApplication::Geometry::Texture;

namespace HelloTriangleApplication::Resources
{
    Result<Unit, ErrorMessage> CubemapExporter::exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData)
    {
        // load cubemap json data
        std::array<StringId, 6> sides;
        nlohmann::json cubemapData = JsonHelper::load(path);
        JsonHelper json = JsonHelper(cubemapData);

        // get all sides of cubemap
        bool foundAllSides = true;
        foundAllSides = foundAllSides && json.trySetProperty("-x", sides[CubeSide::NegX]);
        foundAllSides = foundAllSides && json.trySetProperty("+x", sides[CubeSide::PosX]);
        foundAllSides = foundAllSides && json.trySetProperty("+y", sides[CubeSide::PosY]);
        foundAllSides = foundAllSides && json.trySetProperty("-y", sides[CubeSide::NegY]);
        foundAllSides = foundAllSides && json.trySetProperty("+z", sides[CubeSide::PosZ]);
        foundAllSides = foundAllSides && json.trySetProperty("-z", sides[CubeSide::NegZ]);

        if (!foundAllSides)
            return ErrorMessage({ .channel = "resources" }, "Cubemap {} does not specify all sides.", path.stem().c_str());
        
        // write the sides
        std::ofstream out(getExportPath("cubemaps", path));
        out.write(reinterpret_cast<const char*>(sides.data()), sizeof(StringId) * sides.size());

        // drop sides texture descriptions so they aren't processed as textures
        for (StringId side : sides)
        {
            CHECK(processingData.texDescs->drop(side));
        }

        return Ok();
    }

    Result<Unit, ErrorMessage> CubemapLoader::loadResource(const std::filesystem::path& path,
        std::any resource, RuntimeProcessingData& processingData)
    {
        Cubemap* cubemap = std::any_cast<Cubemap*>(resource);
        std::ifstream in(path);
        in.read(reinterpret_cast<char*>(cubemap->sides.data()), sizeof(StringId) * cubemap->sides.size());

        in.ignore();
        if (!in.eof())
            warn({ .channel = "resources" }, "Read all cubemap information in {} but didn't reach end-of-file.", path);

        return Ok();
    }

    Result<Unit, ErrorMessage> CubemapInitializer::postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData)
    {
        Cubemap* cubemap = std::any_cast<Cubemap*>(resource);

        // configure texture description
        TextureDescription desc = TextureDescription::defaultTexture();
        desc.image = cubemap->sides[0];
        desc.additionalImages.insert(desc.additionalImages.end(), cubemap->sides.begin() + 1, cubemap->sides.end());
        desc.doMipmapping = false;
        desc.viewType = vk::ImageViewType::eCube;
        desc.numLayers = 6;
        desc.createFlags |= vk::ImageCreateFlagBits::eCubeCompatible;

        // create cubemap texture
        auto texResult = Texture::fromDescription(*processingData.app, desc);
        CHECK_KEEP(texResult);
        cubemap->texArray = texResult.take();

        return Ok();
    }
}