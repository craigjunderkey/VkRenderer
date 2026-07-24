#pragma once
#include "../utilities/common.h"
#include "sampler_manager.h"

namespace HelloTriangleApplication::RenderLoop
{
    struct Material
    {
        StringId name;
        StringId diffuseTexture = StringId();
        StringId specularTexture = StringId();
        SamplerName sampler = SamplerName::Repeat;
        vec2 texOffset = vec2(0, 0);
        vec2 texScale = vec2(1.0f, 1.0f);
        vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f);
        vec3 specularColor = vec3(0.9f, 0.9f, 0.9f);
        f32 shininess = 16.0f;
        bool isUnlit = false;
    };
}