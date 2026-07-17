#include "camera.h"

namespace HelloTriangleApplication::RenderLoop
{
    mat4 Camera::view() const
    {
        return glm::lookAt(vec3(0, 2.0f, 3.0f), vec3(0.0f, 0, -1.0f), vec3(0.0f, 1.0f, 0.0f));
    }
}