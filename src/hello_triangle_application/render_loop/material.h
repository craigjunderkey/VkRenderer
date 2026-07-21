#pragma once
#include "../utilities/common.h"
#include "../geometry/texture.h"
#include "sampler_manager.h"

namespace HelloTriangleApplication { class App; }

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
        StringId name;
        const Geometry::Texture& texture;
        SamplerName sampler;
        vec2 texOffset;
        vec2 texScale;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        f32 shininess;
        bool isUnlit;

        void bind(App& app) const;
    };
}