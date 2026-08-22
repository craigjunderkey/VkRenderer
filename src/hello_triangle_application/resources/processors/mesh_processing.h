#pragma once
#include "../resource_type.h"
#include "../../geometry/vertex.h"

namespace HelloTriangleApplication::Resources
{
    class MeshExporter : public ResourceExporter
    {
    public:
        Result<Unit, ErrorMessage> exportResource(
            const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class MeshLoader : public ResourceLoader
    {
    public:
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData) override;
    };

    class MeshImporter
    {
    public:
        virtual Result<std::vector<Geometry::Vertex>, ErrorMessage> importMesh(const std::filesystem::path& path) = 0; 
        virtual ~MeshImporter() = default;
    };

    class ObjImporter : public MeshImporter
    {
        Result<std::vector<Geometry::Vertex>, ErrorMessage> importMesh(const std::filesystem::path& path) override;
    };

    class MeshInitializer : public ResourcePostLoadInitializer
    {
        Result<Unit, ErrorMessage> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData) override;
    };
}