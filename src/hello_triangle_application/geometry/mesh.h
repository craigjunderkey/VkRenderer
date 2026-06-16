#pragma once
#include "../utilities/common.h"
#include "vertex.h"
#include "buffer.h"
#include <concepts>

namespace HelloTriangleApplication::Geometry
{
    template <typename T>
    struct Mesh
    {
        std::vector<Vertex> vertices = {};
        std::vector<T> indices = {};
        FatBuffer vertexBuffer = {};
        FatBuffer indexBuffer = {};
    };
}