#pragma once
#include "../utilities/common.h"
#include "../geometry/texture.h"

namespace HelloTriangleApplication::RenderLoop
{
    struct Light
    {
        vec3 pos;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    
    struct Material
    {
        const Geometry::Texture& texture;
        vec2 texOffset;
        vec2 texScale;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        f32 shininess;
    };
}