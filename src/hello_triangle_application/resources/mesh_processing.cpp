#include "mesh_processing.h"
#include "../geometry/mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <fstream>
#include <sstream>

using HelloTriangleApplication::Geometry::Vertex;
using HelloTriangleApplication::Geometry::Mesh;
using HelloTriangleApplication::Geometry::MeshData;

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> MeshExporter::exportResource(
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
        std::unordered_map<Vertex, u32> uniqueVertices(imported.size());
        std::vector<Vertex> vertices = {};
        vertices.reserve(imported.size());
        std::vector<u32> indices = {};
        indices.reserve(imported.size());

        for (u32 i = 0; i < imported.size(); i++)
        {
            Vertex vertex = imported[i];
            auto [it, inserted] = uniqueVertices.insert({vertex, static_cast<u32>(vertices.size())});
            if (inserted)
            {
                vertices.push_back(std::move(vertex));
            }
            indices.push_back(it->second);
        }

        // drop imported vertices from main memory
        imported = {};

        // output .mesh file
        std::filesystem::path outPath = getExportPath("meshes", path, ".mesh");
        std::ofstream out(outPath, std::ios::binary);
        out << vertices.size() << " " << sizeof(Vertex) << std::endl;
        out << indices.size() << " " << sizeof(u32) << std::endl;
        out.write(reinterpret_cast<const char*>(vertices.data()), sizeof(Vertex) * vertices.size());
        out.write(reinterpret_cast<const char*>(indices.data()), sizeof(u32) * indices.size());

        return Ok();
    }

    Result<Unit, std::string> MeshLoader::loadResource(const std::filesystem::path& path,
        std::any resource, RuntimeProcessingData& processingData)
    {
        // read in vertex/index metadata
        std::ifstream in(path, std::ios::binary);
        u32 numVertices;
        u32 vertexSize;
        u32 numIndices;
        u32 indexSize;
        in >> numVertices >> vertexSize >> numIndices >> indexSize;
        in.ignore();

        // check that vertex and index size match application's
        if (vertexSize != sizeof(Vertex))
        {
            std::ostringstream msg;
            msg << path.string() << " vertex size is " << vertexSize << " but application vertex size is " << sizeof(Vertex) << ".";
            return Err<std::string>(msg.str());
        }

        if (indexSize != sizeof(u32))
        {
            std::ostringstream msg;
            msg << path.string() << " index size is " << indexSize << " but application index size is " << sizeof(u32) << ".";
            return Err<std::string>(msg.str());
        }

        // read in vertex information
        std::vector<Vertex> vertices(numVertices);
        in.read(reinterpret_cast<char*>(vertices.data()), numVertices * vertexSize);

        // read in index information
        std::vector<u32> indices(numIndices);
        in.read(reinterpret_cast<char*>(indices.data()), numIndices * indexSize);

        // check for file completion
        in.ignore();
        if (!in.eof())
        {
            std::ostringstream remainder;
            remainder << in.rdbuf();
            return Err<std::string>("Read all vertex and index information in " + path.string() + " but didn't reach end-of-file.\n" +
                "Remaining: " + remainder.str() + "\n");
        }

        // send information to mesh
        Mesh* mesh = std::any_cast<Mesh*>(resource);
        mesh->name = pathToName(path);
        mesh->data = MeshData {
            .vertices = std::move(vertices),
            .indices = std::move(indices),
            .numIndices = numIndices
        };

        return Ok();
    }

    Result<std::vector<Vertex>, std::string> ObjImporter::importMesh(const std::filesystem::path& path)
    {
        // load the obj file
        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warnText, errText;

        bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnText, &errText, path.c_str());
        
        if (!warnText.empty())
            warn(warnText);

        if (!errText.empty())
            err(errText);

        if (!success)
            return Err<std::string>(errText);
        
        // load vertices from obj faces
        std::vector<Vertex> vertices = {};

        for (const tinyobj::shape_t& shape : shapes)
        {
            for (const tinyobj::index_t index : shape.mesh.indices)
            {
                Vertex vertex {};

                vertex.pos = {
                    attributes.vertices[3 * index.vertex_index + 0],
                    attributes.vertices[3 * index.vertex_index + 1],
                    attributes.vertices[3 * index.vertex_index + 2]
                };

                vertex.normal = {
                    attributes.normals[3 * index.normal_index + 0],
                    attributes.normals[3 * index.normal_index + 1],
                    attributes.normals[3 * index.normal_index + 2]
                };

                vertex.texCoord = {
                    attributes.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attributes.texcoords[2 * index.texcoord_index + 1]
                };

                vertices.emplace_back(std::move(vertex));
            }
        }

        return std::move(vertices);
    }
    
    Result<Unit, std::string> MeshInitializer::postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData)
    {
        Mesh* mesh = std::any_cast<Mesh*>(resource);
        CHECK(mesh->makeBuffers(*processingData.app));
        return Ok();
    }
}