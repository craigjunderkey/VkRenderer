#include "asset_conditioning_pipeline.h"
#include "processors/common_resource_processors.h"
#include "../../utility/string_id_map.h"
#include <chrono>

namespace HelloTriangleApplication::Resources
{
    Result<std::vector<std::filesystem::path>, ErrorMessage> AssetConditioningPipeline::findAssets(const ResourceType& resourceType)
    {
        std::vector<std::filesystem::path> paths = {};

        for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(RES_IMPORT_DIR))
        {
            if (file.is_directory())
                continue;

            std::filesystem::path path = file.path();
            std::filesystem::path extension = path.extension();

            // add file iff it has a matching input file extension
            u32 i = 0;
            bool found = false;
            while (!found && resourceType.inFileExtensions[i] != nullptr && i < MAX_FILE_EXTENSIONS)
            {
                if (strcmp(resourceType.inFileExtensions[i], extension.c_str()) == 0)
                {
                    paths.emplace_back(std::move(path));
                    found = true;
                }
                i++;
            }
        }

        return std::move(paths);
    }

    Result<Unit, ErrorMessage> AssetConditioningPipeline::prepareExportDirectory()
    {
        // clear the export directory
        std::filesystem::remove_all(RES_EXPORT_DIR);
        std::filesystem::create_directory(RES_EXPORT_DIR);

        // create resource output directories
        for (const ResourceType& type : this->types_)
        {
            std::filesystem::path dir = RES_EXPORT_DIR / type.resourceDirectory;
            std::filesystem::create_directory(dir);
        }

        return Ok();
    }

    struct AssetBuildProgress
    {
        std::chrono::milliseconds exportTime;
        std::chrono::milliseconds compileTime;
        std::chrono::milliseconds linkTime;
    };

    void updateResourceCount(const ResourceType& type, OfflineProcessingData& data)
    {
        // get number of resources of this type
        u32 numResources = 0;
        std::filesystem::path resDir = RES_EXPORT_DIR / type.resourceDirectory;
        for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(resDir))
        {
            if (!file.is_directory())
                numResources++;
        }

        // update the table
        data.resourceCounts.add(type.resourceName, numResources);
    }

    Result<Unit, ErrorMessage> AssetConditioningPipeline::buildAssets()
    {
        log({ .channel = "resources" }, "Building resources");
        
        this->prepareExportDirectory();

        CopyExporter defaultExporter;
        NullCompiler defaultCompiler;
        NullLinker defaultLinker;
        NullOfflineTasks defaultTasks;

        OfflineProcessingData processingData;

        // start tracking each resource
        StringIdMap<AssetBuildProgress> buildProgress { "asset_build_progress" };
        for (const ResourceType& type : this->types_)
        {
            buildProgress.add(type.resourceName, {});
        }

        // 0. run build-start tasks
        for (const ResourceType& type : this->types_)
        {
            OfflineResourceTasks* tasks;
            if (type.offline.tasks)
                tasks = type.offline.tasks.value();
            else
                tasks = &defaultTasks;
            CHECK(tasks->onBuildStarted(processingData));
        }

        // 1. export
        for (const ResourceType& type : this->types_)
        {
            auto startTime = std::chrono::high_resolution_clock::now();

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
            CHECK(exporter->begin(processingData));
            for (const std::filesystem::path& path : assetSearchResult.take())
            {
                CHECK(exporter->exportResource(path, processingData));
            }
            CHECK(exporter->end(processingData));

            // update count
            updateResourceCount(type, processingData);

            // store export time
            auto now = std::chrono::high_resolution_clock::now();
            buildProgress.get(type.resourceName).exportTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        }

        // 2. compile
        processingData.resourceCounts.clear();
        for (const ResourceType& type : this->types_)
        {
            auto startTime = std::chrono::high_resolution_clock::now();

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
            CHECK(compiler->begin(processingData));
            std::filesystem::path dir = RES_EXPORT_DIR / type.resourceDirectory;
            for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(dir))
            {
                if (!file.is_directory())
                {
                    CHECK(compiler->compileResource(file.path(), processingData));
                }
            }
            CHECK(compiler->end(processingData));

            // update count
            updateResourceCount(type, processingData);

            // store compile time
            auto now = std::chrono::high_resolution_clock::now();
            buildProgress.get(type.resourceName).compileTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        }

        // 3. link
        processingData.resourceCounts.clear();
        for (const ResourceType& type : this->types_)
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            
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
            CHECK(linker->begin(processingData));
            std::filesystem::path dir = RES_EXPORT_DIR / type.resourceDirectory;
            for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(dir))
            {
                if (!file.is_directory())
                {
                    CHECK(linker->linkResource(file.path(), processingData));
                }
            }
            CHECK(linker->end(processingData));

            // update count
            updateResourceCount(type, processingData);

            // store link time
            auto now = std::chrono::high_resolution_clock::now();
            buildProgress.get(type.resourceName).linkTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        }

        // 4. run build-finish tasks
        for (const ResourceType& type : this->types_)
        {
            OfflineResourceTasks* tasks;
            if (type.offline.tasks)
                tasks = type.offline.tasks.value();
            else
                tasks = &defaultTasks;
            CHECK(tasks->onBuildFinished(processingData));
        }

        // log build duration
        std::stringstream buildOut;
        buildOut << "\n\ttype\texport\tcompile\tlink\ttotal\n";
        for (const ResourceType& type : this->types_)
        {
            const auto& stats = buildProgress.get(type.resourceName);
            std::chrono::milliseconds resTotal = stats.exportTime + stats.compileTime + stats.linkTime;
            buildOut << "\t" << type.resourceName << "\t" << stats.exportTime
                << "\t" << stats.compileTime << "\t" << stats.linkTime
                << "\t" << resTotal << "\n";
        }
        log({ .channel = "resources" }, buildOut.str());

        return Ok();
    }
}