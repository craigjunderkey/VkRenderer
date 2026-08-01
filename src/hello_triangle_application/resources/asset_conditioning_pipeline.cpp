#include "asset_conditioning_pipeline.h"
#include "common_resource_processors.h"

namespace HelloTriangleApplication::Resources
{
    const std::filesystem::path AssetConditioningPipeline::IMPORT_DIRECTORY = "src/assets";
    const std::filesystem::path AssetConditioningPipeline::EXPORT_DIRECTORY = "src/assets_export";

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

    Result<Unit, std::string> AssetConditioningPipeline::buildAssets()
    {
        this->prepareExportDirectory();

        CopyExporter defaultExporter;
        NullCompiler defaultCompiler;
        NullLinker defaultLinker;

        // 1. export
        for (const ResourceType& type : this->types_)
        {
            // get all paths for this resource type
            auto assetSearchResult = this->findAssets(type);
            CHECK_KEEP(assetSearchResult);

            // prepare exporter pointer
            ResourceExporter* exporter;
            if (type.offline.exporter)
            {
                exporter = type.offline.exporter.value();
            }
            else
            {
                defaultExporter = CopyExporter(type);
                exporter = &defaultExporter;
            }

            // export each found asset
            for (const std::filesystem::path& path : assetSearchResult.take())
            {
                std::filesystem::path out = exporter->exportResource(path);
                std::cout << "Exported " << std::string(type.resourceName) << " " << path << " to " << out << std::endl;
            }
        }

        // 2. compile
        for (const ResourceType& type : this->types_)
        {
            // prepare compiler pointer
            ResourceCompiler* compiler;
            if (type.offline.compiler)
            {
                compiler = type.offline.compiler.value();
            }
            else
            {
                defaultCompiler = NullCompiler();
                compiler = &defaultCompiler;
            }

            // compile each exported asset
            std::filesystem::path dir = EXPORT_DIRECTORY / type.resourceDirectory;
            for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(dir))
            {
                std::filesystem::path path = file.path();
                compiler->compileResource(path);
                std::cout << "Compiled " << std::string(type.resourceName) << " " << path << std::endl;
            }
        }

        // 3. link
        for (const ResourceType& type : this->types_)
        {
            // prepare linker pointer
            ResourceLinker* linker;
            if (type.offline.linker)
            {
                linker = type.offline.linker.value();
            }
            else
            {
                defaultLinker = NullLinker();
                linker = &defaultLinker;
            }

            // link each compiled asset
            std::filesystem::path dir = EXPORT_DIRECTORY / type.resourceDirectory;
            for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(dir))
            {
                std::filesystem::path path = file.path();
                linker->linkResource(path);
                std::cout << "Linked " << std::string(type.resourceName) << " " << path << std::endl;
            }
        }

        return Ok();
    }
}