#pragma once
#include "material.h"
#include "../geometry/mesh.h"

namespace HelloTriangleApplication::RenderLoop
{
    class RenderObject
    {
        const Geometry::Mesh<u32>& mesh;
        const Material& material;
        mat4 model;
    };
}