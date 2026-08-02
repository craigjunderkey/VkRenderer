#include "mesh_processing.h"

using HelloTriangleApplication::Geometry::Vertex;

namespace HelloTriangleApplication::Resources
{
    Result<std::filesystem::path, std::string> MeshExporter::exportResource(
        const std::filesystem::path& path, OfflineProcessingData& processingData)
    {
        MeshImporter* importer;

        // get importer depending on file extension
        ObjImporter objImporter;
        std::filesystem::path extension = path.extension();
        if (strcmp(extension.c_str(), ".obj") == 0)
            importer = &objImporter;
        else
            panic("MeshExporter received a file with an invalid extension.");

        // import vertices
        auto importResult = importer->importMesh(path);
        CHECK_KEEP(importResult);
        std::vector<Vertex> imported = importResult.take();

        // generate compressed mesh vertices and indices

        // output .mesh file

        return Err<std::string>("Not implemented : - (");
    }

    Result<Unit, std::string> MeshLoader::loadResource(const std::filesystem::path& path,
        std::any resource, RuntimeProcessingData& processingData)
    {
        // do nothing for now
        return Ok();
    }

    Result<std::vector<Vertex>, std::string> ObjImporter::importMesh(const std::filesystem::path& path)
    {
        std::vector<Vertex> vertices = {};
        return std::move(vertices);
    }
}