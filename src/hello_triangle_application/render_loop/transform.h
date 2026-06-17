#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    struct Transform
    {
        vec3 t;
        vec3 r;
        vec3 s;

        mat4 model();
    };

    void createViewTransform(App& app);
    void createProjectionTransform(App& app);
}