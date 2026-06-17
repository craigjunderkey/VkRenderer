#include "../hello_triangle_application.h"
#include "transform.h"

#include <numbers>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

constexpr f32 QUARTER_PI = std::numbers::pi / 4.0f;

namespace HelloTriangleApplication::RenderLoop
{
    void createViewTransform(App& app)
    {
        app.view = glm::lookAt(vec3(0, 2.0f, 3.0f), vec3(0.0f, 0, -1.0f), vec3(0.0f, 1.0f, 0.0f));
    }

    void createProjectionTransform(App& app)
    {
        app.projection = glm::perspective(
            QUARTER_PI,
            static_cast<f32>(app.swapChainExtent.width) / static_cast<f32>(app.swapChainExtent.height),
            0.1f, 10.0f
        );
    }
}