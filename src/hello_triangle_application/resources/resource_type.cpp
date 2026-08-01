#include "resource_type.h"
#include "asset_conditioning_pipeline.h"

namespace HelloTriangleApplication::Resources
{
    std::filesystem::path ResourceExporter::getExportDirectory(const char* resourceDirectory)
    {
        return AssetConditioningPipeline::EXPORT_DIRECTORY / resourceDirectory;
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
        return AssetConditioningPipeline::EXPORT_DIRECTORY / resourceDirectory / name;
    }
}