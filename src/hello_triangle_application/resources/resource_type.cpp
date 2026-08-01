#include "resource_type.h"

namespace HelloTriangleApplication::Resources
{
    const std::filesystem::path ResourceExporter::EXPORT_DIRECTORY = "src/assets_export";

    std::filesystem::path ResourceExporter::getExportDirectory(const char* resourceDirectory)
    {
        return EXPORT_DIRECTORY / resourceDirectory;
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
        return EXPORT_DIRECTORY / resourceDirectory / name;
    }
}