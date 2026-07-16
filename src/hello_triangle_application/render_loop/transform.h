#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    struct Transform
    {
        vec3 t = vec3(0.0, 0.0, 0.0);
        vec3 r = vec3(0.0, 0.0, 0.0);
        vec3 s = vec3(1.0, 1.0, 1.0);

        mat4 model();
    };

    void createViewTransform(App& app);
    void createProjectionTransform(App& app);
}