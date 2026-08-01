#include "camera.h"
#include "transform.h"

constexpr f32 X_LIMIT = (glm::pi<f32>() / 2.0f) - (10.0f * glm::epsilon<f32>());

namespace HelloTriangleApplication::RenderLoop
{
    mat4 Camera::view() const
    {
        vec3 lookVec = this->basis() * vec4(FORWARD, 0.0f);
        return glm::lookAt(this->xform_.pos, this->xform_.pos + lookVec, UP);
    }

    mat4 Camera::basis() const
    {
        mat4 yRotation = glm::rotate(mat4(1.0f), -this->xform_.yRot, UP);
        mat4 xRotation = glm::rotate(mat4(1.0f), this->xform_.xRot, SIDE);
        return yRotation * xRotation;
    }

    CameraTransform Camera::transform() const
    {
        return this->xform_;
    }

    void Camera::setTransform(CameraTransform transform)
    {
        transform.xRot = glm::clamp<f32>(transform.xRot, -X_LIMIT, X_LIMIT);
        this->xform_ = transform;
    }
}