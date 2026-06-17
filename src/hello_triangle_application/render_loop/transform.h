#pragma once
#include "../utilities/common.h"
#include <glm/gtx/euler_angles.hpp>

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    struct Transform
    {
        vec3 t;
        vec3 r;
        vec3 s;

        mat4 model()
        {
            mat4 model = mat4(1.0f);
            model = glm::translate(model, this->t);
            model = glm::eulerAngleXYZ(this->r.x, this->r.y, this->r.z) * model;
            model = glm::scale(model, this->s);
            return std::move(model);
        }
    };

    void createViewTransform(App& app);
    void createProjectionTransform(App& app);
}