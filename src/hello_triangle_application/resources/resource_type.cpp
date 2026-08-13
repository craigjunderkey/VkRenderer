#include "resource_type.h"
#include "asset_conditioning_pipeline.h"

#define RESOURCE_NAME_HERE "Resource type " + std::string(resourceName)

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> ResourceType::check()
    {
        if (resourceName == nullptr)
            return Err<std::string>("Resource type missing a name.");
        if (resourceDirectory == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " missing a directory.");
        if (inFileExtensions[0] == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " is missing a first input file extension.");
        if (offline.exporter.has_value() && offline.exporter.value() == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " has an exporter but it's nullptr.");
        if (offline.compiler.has_value() && offline.compiler.value() == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " has a compiler but it's nullptr.");
        if (offline.linker.has_value() && offline.linker.value() == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " has a linker but it's nullptr.");
        if (offline.tasks.has_value() && offline.tasks.value() == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " has offline tasks but it's nullptr.");
        if (runtime.loader == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " is missing a loader.");
        if (runtime.unloader.has_value() && runtime.unloader.value() == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " has an unloader but it's nullptr.");
        if (runtime.postLoadInitializer.has_value() && runtime.postLoadInitializer.value() == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " has a post-load initializer but it's nullptr.");
        if (runtime.tasks.has_value() && runtime.tasks.value() == nullptr)
            return Err<std::string>(RESOURCE_NAME_HERE + " has runtime tasks but it's nullptr.");

        return Ok();
    }
    
    std::filesystem::path getResDir(const char* resourceDirectory)
    {
        return std::move(RES_EXPORT_DIR / resourceDirectory);
    }
    
    std::filesystem::path ResourceExporter::getExportDirectory(const char* resourceDirectory)
    {
        return std::move(getResDir(resourceDirectory));
    }

    std::filesystem::path ResourceExporter::getExportPath(const char* resourceDirectory,
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

    std::filesystem::path ResourceCompiler::getResourceDirectory(const char* resourceDirectory)
    {
        return std::move(getResDir(resourceDirectory));
    }

    std::filesystem::path ResourceLinker::getResourceDirectory(const char* resourceDirectory)
    {
        return std::move(getResDir(resourceDirectory));
    }
}