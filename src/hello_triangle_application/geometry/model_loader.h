#pragma once
#include "../utilities/common.h"
#include "vertex.h"
#include "mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <unordered_map>

namespace HelloTriangleApplication::Geometry
{
    template<typename T>
    Result<MeshData<T>, std::string> loadMeshData(std::string path)
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

        // make a mesh from the faces 
        MeshData<T> mesh {};

        std::unordered_map<Vertex, T> uniqueVertices {};

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

                auto [it, inserted] = uniqueVertices.insert({vertex, static_cast<T>(mesh.vertices.size())});
                if (inserted)
                {
                    mesh.vertices.push_back(std::move(vertex));                    
                }
                mesh.indices.push_back(it->second);
            }
        }

        return std::move(mesh);
    }
}