#include "resource_type.h"
#include "asset_conditioning_pipeline.h"

namespace VkRenderer::Resources
{
    Result<Unit, ErrorMessage> ResourceType::check()
    {
        if (resourceName == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type missing a name.");
        if (inFileExtensions[0] == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} is missing a first input file extension.", resourceName);
        if (offline.exporter.has_value() && offline.exporter.value() == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} has an exporter but it's nullptr.", resourceName);
        if (offline.compiler.has_value() && offline.compiler.value() == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} has a compiler but it's nullptr.", resourceName);
        if (offline.linker.has_value() && offline.linker.value() == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} has a linker but it's nullptr.", resourceName);
        if (offline.tasks.has_value() && offline.tasks.value() == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} has offline tasks but it's nullptr.", resourceName);
        if (runtime.loader == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} is missing a loader.", resourceName);
        if (runtime.unloader.has_value() && runtime.unloader.value() == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} has an unloader but it's nullptr.", resourceName);
        if (runtime.postLoadInitializer.has_value() && runtime.postLoadInitializer.value() == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} has a post-load initializer but it's nullptr.", resourceName);
        if (runtime.tasks.has_value() && runtime.tasks.value() == nullptr)
            return ErrorMessage({ .channel = "resources" }, "Resource type {} has runtime tasks but it's nullptr.", resourceName);

        return Ok();
    }
    
    static std::filesystem::path getResDir(const std::filesystem::path& resourceDirectory)
    {
        return RES_EXPORT_DIR / resourceDirectory;
    }
    
    std::filesystem::path ResourceExporter::getExportDirectory(const std::filesystem::path& resourceDirectory)
    {
        return getResDir(resourceDirectory);
    }

    std::filesystem::path ResourceExporter::getExportPath(const std::filesystem::path& resourceDirectory,
        const std::filesystem::path& assetPath, Option<const char*> newFileExtension)
    {
        // get file extension
        std::filesystem::path fileExtension;
        if (newFileExtension)
            fileExtension = newFileExtension.value();
        else
            fileExtension = assetPath.extension();
        
        // get output file name
        std::filesystem::path name = assetPath.filename();
        name.replace_extension(fileExtension);
        
        // generate export path
        return RES_EXPORT_DIR / resourceDirectory / name;
    }

    std::filesystem::path ResourceCompiler::getResourceDirectory(const std::filesystem::path& resourceDirectory)
    {
        return getResDir(resourceDirectory);
    }

    std::filesystem::path ResourceLinker::getResourceDirectory(const std::filesystem::path& resourceDirectory)
    {
        return getResDir(resourceDirectory);
    }
}