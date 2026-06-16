#pragma once
#include "../utilities/common.h"
#include "vertex.h"
#include "buffer.h"

namespace HelloTriangleApplication::Geometry
{
    template<typename T>
    struct MeshData
    {
        std::vector<Vertex> vertices = {};
        std::vector<T> indices = {};
    };
    
    template <typename T>
    struct Mesh
    {
        std::vector<Vertex> vertices = {};
        std::vector<T> indices = {};
        FatBuffer vertexBuffer = {};
        FatBuffer indexBuffer = {};

        static Mesh fromData(MeshData<T> data)
        {
            Mesh<T> mesh {
                .vertices = std::move(data.vertices),
                .indices = std::move(data.indices)
            };
            return mesh;
        }
    };
}