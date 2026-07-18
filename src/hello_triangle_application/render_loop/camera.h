#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication::RenderLoop
{
    struct CameraTransform
    {
        vec3 pos;
        f32 xRot;
        f32 yRot;
    };
    
    class Camera
    {
    private:
        CameraTransform xform_;

        const vec3 FORWARD = vec3(0.0f, 0.0f, -1.0f);
        const vec3 UP = vec3(0.0f, 1.0f, 0.0f);
        const vec3 SIDE = vec3(1.0f, 0.0f, 0.0f);
    
    public:
        Camera() = default;
        Camera(vec3 pos, f32 xRot, f32 yRot) : xform_(CameraTransform { .pos = pos, .xRot = xRot, .yRot = yRot }) {}
        Camera(CameraTransform transform) : xform_(transform) {}

        mat4 view() const;
        CameraTransform transform() const;
        void setTransform(CameraTransform transform);
    };
}