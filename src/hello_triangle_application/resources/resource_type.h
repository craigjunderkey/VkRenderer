#pragma once
#include "../utilities/common.h"
#include <filesystem>
#include <any>
#include "processing_data.h"

#define MAX_FILE_EXTENSIONS 4

namespace VkRenderer::Resources
{
    class ResourceExporter;
    class ResourceCompiler;
    class ResourceLinker;
    class OfflineResourceTasks;
    class ResourceLoader;
    class ResourceUnloader;
    class ResourcePostLoadInitializer;
    class RuntimeResourceTasks;

    static const std::filesystem::path RES_IMPORT_DIR = "assets";
    static const std::filesystem::path RES_EXPORT_DIR = "assets_export";
    
    inline StringId pathToName(const std::filesystem::path& path)
    {
        return StringId(path.stem().string());
    }
    
    struct OfflineResourceProcessors
    {
        Option<ResourceExporter*> exporter;
        Option<ResourceCompiler*> compiler;
        Option<ResourceLinker*> linker;
        Option<OfflineResourceTasks*> tasks;
    };

    struct RuntimeResourceProcessors
    {
        ResourceLoader* loader;
        Option<ResourceUnloader*> unloader;
        Option<ResourcePostLoadInitializer*> postLoadInitializer;
        Option<RuntimeResourceTasks*> tasks;
    };

    struct ResourceType
    {
        const char* resourceName;
        const char* resourceDirectory;                      // name of the export sub-directory
        const char* inFileExtensions[MAX_FILE_EXTENSIONS];  // file extensions must begin with `.`
        OfflineResourceProcessors offline;
        RuntimeResourceProcessors runtime;

        Result<Unit, ErrorMessage> check();
    };

    class ResourceExporter
    {
    public:
        virtual Result<Unit, ErrorMessage> begin(OfflineProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, ErrorMessage> end(OfflineProcessingData& processingData) { return Ok(); }

        virtual Result<Unit, ErrorMessage> exportResource(
            const std::filesystem::path& path, OfflineProcessingData& processingData) = 0;
        
        virtual ~ResourceExporter() = default;
    
    protected:
        // resourceDirectory is the `resourceDirectory` field of the resource's ResourceType
        std::filesystem::path getExportDirectory(const char* resourceDirectory);

        // resourceDirectory is the `resourceDirectory` field of the resource's ResourceType
        // assetPath is the path supplied to `exportResource`
        std::filesystem::path getExportPath(const char* resourceDirectory,
            const std::filesystem::path& assetPath, Option<const char*> newFileExtension = {});
    };

    class ResourceCompiler
    {
    public:
        virtual Result<Unit, ErrorMessage> begin(OfflineProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, ErrorMessage> end(OfflineProcessingData& processingData) { return Ok(); }

        virtual Result<Unit, ErrorMessage> compileResource(const std::filesystem::path& path, OfflineProcessingData& processingData) = 0;

        virtual ~ResourceCompiler() = default;
    
    protected:
        std::filesystem::path getResourceDirectory(const char* resourceDirectory);
    };

    class ResourceLinker
    {
    public:
        virtual Result<Unit, ErrorMessage> begin(OfflineProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, ErrorMessage> end(OfflineProcessingData& processingData) { return Ok(); }

        virtual Result<Unit, ErrorMessage> linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) = 0;
        
        virtual ~ResourceLinker() = default;
    
    protected:
        std::filesystem::path getResourceDirectory(const char* resourceDirectory);
    };

    class OfflineResourceTasks
    {
    public:
        virtual Result<Unit, ErrorMessage> onBuildStarted(OfflineProcessingData& processingData) = 0;
        virtual Result<Unit, ErrorMessage> onBuildFinished(OfflineProcessingData& processingData) = 0;

        virtual ~OfflineResourceTasks() = default;
    };

    class ResourceLoader
    {
    public:
        virtual Result<Unit, ErrorMessage> begin(RuntimeProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, ErrorMessage> end(RuntimeProcessingData& processingData) { return Ok(); }

        // std::any resource must be a pointer to an instance of the resource type, which loadResource
        // will load data into using the file described by the path
        virtual Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData) = 0;
        
        virtual ~ResourceLoader() = default;
    };

    class ResourceUnloader
    {
    public:
        virtual Result<Unit, ErrorMessage> begin(RuntimeProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, ErrorMessage> end(RuntimeProcessingData& processingData) { return Ok(); }

        // std::any resource must be a pointer to an instance of the resource type to unload
        virtual Result<Unit, ErrorMessage> unloadResource(std::any resource, RuntimeProcessingData& processingData) = 0;
        
        virtual ~ResourceUnloader() = default;
    };

    class ResourcePostLoadInitializer
    {
    public:
        virtual Result<Unit, ErrorMessage> begin(RuntimeProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, ErrorMessage> end(RuntimeProcessingData& processingData) { return Ok(); }

        // std::any resource must be a pointer to an instance of the resource type to post-load initialize
        virtual Result<Unit, ErrorMessage> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData) = 0;
        
        virtual ~ResourcePostLoadInitializer() = default;
    };

    class RuntimeResourceTasks
    {
    public:
        virtual Result<Unit, ErrorMessage> onStartup(RuntimeProcessingData& processingData) = 0;
        virtual Result<Unit, ErrorMessage> onShutdown(RuntimeProcessingData& processingData) = 0;

        virtual ~RuntimeResourceTasks() = default;
    };
}
