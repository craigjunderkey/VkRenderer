#pragma once
#include "../utilities/common.h"
#include <iostream>

namespace HelloTriangleApplication::RenderLoop
{
    struct MaterialResourceData
    {
        StringId name;
        StringId texture = StringId();
        vec2 texOffset = vec2(0, 0);
        vec2 texScale = vec2(1.0f, 1.0f);
        vec3 ambient = vec3(1.0f, 1.0f, 1.0f);
        vec3 diffuse = vec3(1.0f, 1.0f, 1.0f);
        vec3 specular = vec3(0.9f, 0.9f, 0.9f);
        f32 shininess = 50.0f;
        bool isUnlit = false;
    };

    #ifndef NDEBUG
    inline std::ostream& operator<<(std::ostream& os, const MaterialResourceData& mat)
    {
        return os << mat.name.string() << ": {\n" << "\ttexture: " << mat.texture.string() << ",\n"
            << "\ttexOffset: " << mat.texOffset << ",\n" << "\ttexScale: " << mat.texScale << ",\n"
            << "\tambient: " << mat.ambient << ",\n" << "\tdiffuse: " << mat.diffuse << ",\n"
            << "\tspecular: " << mat.specular << ",\n" << "\tshininess: " << mat.shininess << ",\n"
            << "\tisUnlit: " << mat.isUnlit << "\n}";
    }
    #endif
}