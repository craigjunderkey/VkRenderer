#pragma once
#include "../utilities/common.h"

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 64
#define MAX_SPOT_LIGHTS 16

struct LightColorComponents
{
    vec3 ambient    alignas(16) = vec3(0.1f, 0.1f, 0.1f);
    vec3 diffuse    alignas(16) = vec3(0.8f, 0.8f, 0.8f);
    vec3 specular   alignas(16) = vec3(0.9f, 0.9f, 0.9f);
};

struct PointLight
{
    void setWorldPos(const mat4& viewMatrix, vec3 worldPos)
    {
        this->position = viewMatrix * vec4(worldPos, 1.0);
    }

    vec3 position alignas(16);
    LightColorComponents components;
};

struct Environment
{
    std::array<PointLight, MAX_POINT_LIGHTS> pointLights;
    u32 numPointLights alignas(4);
};