#include "texture_processing.h"
#include "../texture_description.h"
#include "../../geometry/texture.h"
#include "../io_helper.h"

namespace VkRenderer::Resources
{
    void writeTexDesc(std::ofstream& out, const TextureDescription& desc)
    {
        // write everything up until the `additionalImages` vector
        out.write(reinterpret_cast<const char*>(&desc), offsetof(TextureDescription, additionalImages));

        // write `additionalImages` vector
        writeVec<StringId>(out, desc.additionalImages);
    }

    void readTexDesc(std::ifstream& in, TextureDescription& desc)
    {
        // read everything up until the `additionalImages` vector
        in.read(reinterpret_cast<char*>(&desc), offsetof(TextureDescription, additionalImages));

        // read `additionalImages` vector
        readVec<StringId>(in, desc.additionalImages);
    }
    
    Result<Unit, ErrorMessage> TextureExporter::exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData)
    {
        StringId name = pathToName(path);
        TextureDescription& desc = processingData.texDescs->getOrAdd(name);
        desc.image = name;

        std::filesystem::path outPath = getExportPath("textures", path, ".texdesc");
        auto out = std::ofstream(outPath);
        processingData.texDescs->attachPath(name, std::move(outPath));
        writeTexDesc(out, desc);
        
        return Ok();
    }

    Result<Unit, ErrorMessage> TextureLinker::linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData)
    {
        TextureDescription& desc = processingData.texDescs->getOrAdd(pathToName(path));
        auto file = std::ofstream(path, std::ios::binary);
        writeTexDesc(file, desc);
        return Ok();
    }

    Result<Unit, ErrorMessage> TextureLoader::loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData)
    {
        StringId name = pathToName(path);
        
        // name texture
        Geometry::Texture* tex = std::any_cast<Geometry::Texture*>(resource);
        tex->name = name;

        // load texture description
        TextureDescription& desc = processingData.texDescs->getOrAdd(name);
        auto in = std::ifstream(path);
        readTexDesc(in, desc);
        in.ignore();
        if (!in.eof())
            warn({ .channel = "resources" }, "Read all texture info in {} but didn't reach end-of-file.", path);
        
        return Ok();
    }

    Result<Unit, ErrorMessage> TextureInitializer::postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData)
    {
        Geometry::Texture* tex = std::any_cast<Geometry::Texture*>(resource); 
        auto loadResult = Geometry::Texture::fromDescription(*processingData.app, processingData.texDescs->descs().get(tex->name));
        CHECK_KEEP(loadResult);
        *tex = loadResult.take();
        return Ok();
    }

    Result<Unit, ErrorMessage> TextureInitializer::end(RuntimeProcessingData& processingData)
    {
        processingData.texDescs->descs().clear();
        return Ok();
    }

    Result<Unit, ErrorMessage> OfflineTextureTasks::onBuildStarted(OfflineProcessingData& processingData)
    {
        processingData.texDescs = new TextureDescriptionManager();
        return Ok();
    }

    Result<Unit, ErrorMessage> OfflineTextureTasks::onBuildFinished(OfflineProcessingData& processingData)
    {
        delete processingData.texDescs;
        return Ok();
    }

    Result<Unit, ErrorMessage> RuntimeTextureTasks::onStartup(RuntimeProcessingData& processingData)
    {
        processingData.texDescs = new TextureDescriptionManager();
        return Ok();
    }

    Result<Unit, ErrorMessage> RuntimeTextureTasks::onShutdown(RuntimeProcessingData& processingData)
    {
        delete processingData.texDescs;
        return Ok();
    }
}