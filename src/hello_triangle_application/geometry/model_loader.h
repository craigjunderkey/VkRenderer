#pragma once
#include "../utilities/common.h"
#include "vertex.h"
#include "mesh.h"

namespace HelloTriangleApplication::Geometry
{
    Result<MeshData, std::string> loadMeshData(std::string path)
    {
        return MeshData {};
    }
}