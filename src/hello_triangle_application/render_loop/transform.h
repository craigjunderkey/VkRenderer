#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    struct Trs
    {
        vec3 t = vec3(0.0f, 0.0f, 0.0f);
        vec3 r = vec3(0.0f, 0.0f, 0.0f);
        vec3 s = vec3(1.0f, 1.0f, 1.0f);

        mat4 model() const;
    };

    struct Transform
    {
        vec3 t = vec3(0.0f, 0.0f, 0.0f);
        vec3 r = vec3(0.0f, 0.0f, 0.0f);
        vec3 s = vec3(1.0f, 1.0f, 1.0f);
        Option<Transform*> parent = {};

        Trs trs() const;
        mat4 local() const;
        mat4 global() const;

        void setFromTrs(const Trs& trs);
    };

    void createProjectionTransform(App& app);
}