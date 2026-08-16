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
        vec3 specularColor = vec3(1.0f, 1.0f, 1.0f);
        f32 shininess = 16.0f;
        bool isUnlit = false;
    };

    struct MaterialUniformData
    {
        i32 diffMap     alignas(4);
        i32 specMap     alignas(4);
        u32 samplerId   alignas(4);
        f32 shininess   alignas(4);
        bool isUnlit    alignas(4);
        vec2 texOffset  alignas(8);
        vec2 texScale   alignas(8);
        vec3 diffColor  alignas(16);
        vec3 specColor  alignas(16);
    };
}