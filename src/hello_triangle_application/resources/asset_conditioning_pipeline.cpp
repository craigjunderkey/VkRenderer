#include "asset_conditioning_pipeline.h"
#include "common_resource_processors.h"

namespace HelloTriangleApplication::Resources
{
    const std::filesystem::path AssetConditioningPipeline::IMPORT_DIRECTORY = "src/assets";
    const std::filesystem::path AssetConditioningPipeline::EXPORT_DIRECTORY = "src/assets_export";

    struct ProcessorPtrs
    {
        ResourceExporter* exporter;
    };

    Result<std::vector<std::filesystem::path>, std::string> AssetConditioningPipeline::findAssets(const ResourceType& resourceType)
    {
        std::vector<std::filesystem::path> paths = {};

        for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(IMPORT_DIRECTORY))
        {
            std::filesystem::path path = file.path();
            std::filesystem::path extension = path.extension();

            // add file iff it has a matching input file extension
            u32 i = 0;
            bool found = false;
            while (!found && resourceType.inFileExtensions[i] != nullptr && i < MAX_FILE_EXTENSIONS)
            {
                if (strcmp(resourceType.inFileExtensions[i], extension.c_str()) == 0)
                {
                    std::cout << "Found " << std::string(resourceType.resourceName) << " " << path << std::endl;
                    paths.emplace_back(std::move(path));
                    found = true;
                }
                i++;
            }
        }

        return std::move(paths);
    }

    Result<Unit, std::string> AssetConditioningPipeline::prepareExportDirectory()
    {
        // clear the export directory
        std::filesystem::remove_all(EXPORT_DIRECTORY);
        std::filesystem::create_directory(EXPORT_DIRECTORY);

        // create resource output directories
        for (const ResourceType& type : this->types_)
        {
            std::filesystem::path dir = EXPORT_DIRECTORY / type.resourceDirectory;
            std::filesystem::create_directory(dir);
        }

        return Ok();
    }

    ProcessorPtrs getProcessorPointers(const ResourceType& type, CopyExporter& defaultExporter)
    {
        ProcessorPtrs ptrs;

        // exporter
        if (type.offline.exporter)
        {
            ptrs.exporter = type.offline.exporter.value();
        }
        else
        {
            defaultExporter = CopyExporter(type);
            ptrs.exporter = &defaultExporter;
        }

        return std::move(ptrs);
    }

    Result<Unit, std::string> AssetConditioningPipeline::buildAssets()
    {
        this->prepareExportDirectory();

        CopyExporter defaultExporter;

        for (const ResourceType& type : this->types_)
        {
            // get all paths for this resource type
            auto assetSearchResult = this->findAssets(type);
            CHECK_KEEP(assetSearchResult);

            // prepare pointers
            ProcessorPtrs ptrs = getProcessorPointers(type, defaultExporter);

            for (const std::filesystem::path& path : assetSearchResult.take())
            {
                // 1. export
                std::filesystem::path out = ptrs.exporter->exportResource(path);
                std::cout << "Exported " << std::string(type.resourceName) << " " << path << " to " << out << std::endl;
            }
        }

        return Ok();
    }
}