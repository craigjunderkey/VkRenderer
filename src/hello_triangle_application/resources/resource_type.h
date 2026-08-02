#pragma once
#include "../utilities/common.h"
#include <filesystem>
#include <any>

#define MAX_FILE_EXTENSIONS 4

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Resources
{
    class ResourceExporter;
    class ResourceCompiler;
    class ResourceLinker;
    class ResourceLoader;
    class ResourceUnloader;
    class ResourcePostLoadInitializer;

    static const std::filesystem::path RES_IMPORT_DIR = "src/assets";
    static const std::filesystem::path RES_EXPORT_DIR = "src/assets_export";
    
    inline StringId pathToName(const std::filesystem::path& path)
    {
        return StringId(path.stem().string());
    }
    
    struct OfflineResourceProcessors
    {
        Option<ResourceExporter*> exporter;
        Option<ResourceCompiler*> compiler;
        Option<ResourceLinker*> linker;
    };

    struct OfflineProcessingData
    {
    };

    struct RuntimeResourceProcessors
    {
        ResourceLoader* loader;
        Option<ResourceUnloader*> unloader;
        Option<ResourcePostLoadInitializer*> postLoadInitializer;
    };

    struct RuntimeProcessingData
    {
        App* app;
    };

    struct ResourceType
    {
        const char* resourceName;
        const char* resourceDirectory;                      // name of the export sub-directory
        const char* inFileExtensions[MAX_FILE_EXTENSIONS];  // file extensions must begin with `.`
        OfflineResourceProcessors offline;
        RuntimeResourceProcessors runtime;

        Result<Unit, std::string> isValid();
    };

    class ResourceExporter
    {
    public:
        virtual Result<std::filesystem::path, std::string> exportResource(
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
        virtual Result<Unit, std::string> compileResource(const std::filesystem::path& path, OfflineProcessingData& processingData) = 0;
        virtual ~ResourceCompiler() = default;
    };

    class ResourceLinker
    {
    public:
        virtual Result<Unit, std::string> linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) = 0;
        virtual ~ResourceLinker() = default;
    };

    class ResourceLoader
    {
    public:
        virtual Result<Unit, std::string> begin(RuntimeProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, std::string> end(RuntimeProcessingData& processingData) { return Ok(); }

        // std::any resource must be a pointer to an instance of the resource type, which loadResource
        // will load data into using the file described by the path
        virtual Result<Unit, std::string> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData) = 0;
        
        virtual ~ResourceLoader() = default;
    };

    class ResourceUnloader
    {
    public:
        virtual Result<Unit, std::string> begin(RuntimeProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, std::string> end(RuntimeProcessingData& processingData) { return Ok(); }

        // std::any resource must be a pointer to an instance of the resource type to unload
        virtual Result<Unit, std::string> unloadResource(std::any resource, RuntimeProcessingData& processingData) = 0;
        
        virtual ~ResourceUnloader() = default;
    };

    class ResourcePostLoadInitializer
    {
    public:
        virtual Result<Unit, std::string> begin(RuntimeProcessingData& processingData) { return Ok(); }
        virtual Result<Unit, std::string> end(RuntimeProcessingData& processingData) { return Ok(); }

        // std::any resource must be a pointer to an instance of the resource type to post-load initialize
        virtual Result<Unit, std::string> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData) = 0;
        
        virtual ~ResourcePostLoadInitializer() = default;
    };
}