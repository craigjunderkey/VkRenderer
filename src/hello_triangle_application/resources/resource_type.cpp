#include "resource_type.h"
#include "asset_conditioning_pipeline.h"

#define RESOURCE_NAME_HERE "Resource type " + std::string(resourceName)

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> ResourceType::isValid()
    {
        if (resourceName == nullptr) panic("Resource type missing a name.");
        if (resourceDirectory == nullptr) panic(RESOURCE_NAME_HERE + " missing a directory.");
        if (inFileExtensions == nullptr) panic(RESOURCE_NAME_HERE + " has no array of input file extensions.");
        if (inFileExtensions[0] == nullptr) panic(RESOURCE_NAME_HERE + " is missing a first input file extension.");
        if (offline.exporter.has_value() && offline.exporter.value() == nullptr)
            panic(RESOURCE_NAME_HERE + " has an exporter but it's nullptr.");
        if (offline.compiler.has_value() && offline.compiler.value() == nullptr)
            panic(RESOURCE_NAME_HERE + " has a compiler but it's nullptr.");
        if (offline.linker.has_value() && offline.linker.value() == nullptr)
            panic(RESOURCE_NAME_HERE + " has a linker but it's nullptr.");
        if (runtime.loader == nullptr) panic(RESOURCE_NAME_HERE + " is missing a loader.");
        if (runtime.unloader.has_value() && runtime.unloader.value() == nullptr)
            panic(RESOURCE_NAME_HERE + " has an unloader but it's nullptr.");

        return Ok();
    }
    
    std::filesystem::path ResourceExporter::getExportDirectory(const char* resourceDirectory)
    {
        return RES_EXPORT_DIR / resourceDirectory;
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
}