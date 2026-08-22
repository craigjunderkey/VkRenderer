#pragma once
#include "../utilities/common.h"
#include "vertex.h"
#include "buffer.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    struct MeshData
    {
        std::vector<Vertex> vertices = {};
        std::vector<u32> indices = {};
        u32 numIndices = 0;
    };
    
    struct Mesh
    {
        StringId name = StringId();
        MeshData data = {};
        FatBuffer vertexBuffer = {};
        FatBuffer indexBuffer = {};

        static std::vector<u32> queueFamilies(App& app);
        Result<Unit, ErrorMessage> makeBuffers(App& app);
    };
}