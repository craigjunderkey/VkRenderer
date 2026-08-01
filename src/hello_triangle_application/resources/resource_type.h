#pragma once
#include "../utilities/common.h"
#include <filesystem>
#include <any>

#define MAX_FILE_EXTENSIONS 4

namespace HelloTriangleApplication::Resources
{
    class ResourceExporter;
    class ResourceCompiler;
    class ResourceLinker;
    class ResourceLoader;
    class ResourceUnloader;

    struct OfflineResourceProcessors
    {
        Option<ResourceExporter*> exporter;
        Option<ResourceCompiler*> compiler;
        Option<ResourceLinker*> linker;
    };

    struct RuntimeResourceProcessors
    {
        ResourceLoader* loader;
        Option<ResourceUnloader*> unloader;
    };

    struct ResourceType
    {
        const char* resourceName;
        const char* resourceDirectory;                      // name of the export sub-directory
        const char* inFileExtensions[MAX_FILE_EXTENSIONS];  // file extensions must begin with `.`
        const char* outFileExtension;
        OfflineResourceProcessors offline;
        RuntimeResourceProcessors runtime;
    };

    class ResourceExporter
    {
    public:
        virtual std::filesystem::path exportResource(const std::filesystem::path& path) = 0;
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
        virtual void compileResource(const std::filesystem::path& path) = 0;
        virtual ~ResourceCompiler() = default;
    };

    class ResourceLinker
    {
    public:
        virtual void linkResource(const std::filesystem::path& path) = 0;
        virtual ~ResourceLinker() = default;
    };

    class ResourceLoader
    {
    public:
        virtual void begin() {}
        virtual void end() {}

        // std::any resource must be a pointer to an instance of the resource type, which loadResource
        // will load data into using the file described by the path
        virtual Result<Unit, std::string> loadResource(const std::filesystem::path path, std::any resource) = 0;
        
        virtual ~ResourceLoader() = default;
    };

    class ResourceUnloader
    {
    public:
        // std::any resource must be a pointer to an instance of the resource type to unload
        virtual Result<Unit, std::string> unloadResource(std::any resource) = 0;
        virtual ~ResourceUnloader() = default;
    };
}