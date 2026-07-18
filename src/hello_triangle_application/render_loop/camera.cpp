#include "camera.h"

#include <numbers>

constexpr f32 HALF_PI = std::numbers::pi / 2.0f;

namespace HelloTriangleApplication::RenderLoop
{
    mat4 Camera::view() const
    {
        vec3 lookVec = glm::rotate(mat4(1.0f), this->xform_.xRot, SIDE)
            * glm::rotate(mat4(1.0f), -this->xform_.yRot, UP)
            * vec4(FORWARD, 1.0f);
        return glm::lookAt(-this->xform_.pos, -this->xform_.pos + lookVec, UP);
    }

    CameraTransform Camera::transform() const
    {
        return this->xform_;
    }

    void Camera::setTransform(CameraTransform transform)
    {
        transform.xRot = glm::clamp<f32>(transform.xRot, -HALF_PI, HALF_PI);
        this->xform_ = transform;
    }
}