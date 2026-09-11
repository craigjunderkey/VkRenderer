#pragma once
#include "../utilities/common.h"

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 64
#define MAX_SPOT_LIGHTS 16

struct DebugLightPushConstants
{
    mat4 mvp    alignas(16);
    vec4 color  alignas(16);
};

struct LightColorComponents
{
    vec3 ambient    alignas(16) = vec3(0.1f, 0.1f, 0.1f);
    vec3 diffuse    alignas(16) = vec3(0.8f, 0.8f, 0.8f);
    vec3 specular   alignas(16) = vec3(0.9f, 0.9f, 0.9f);
};

struct PointLight
{
    vec3 posWorld                   alignas(16);
    LightColorComponents components alignas(16);
    vec3 attenuationFactors         alignas(16);
};

struct DirectionalLight
{
    vec3 dirWorld                   alignas(16);
    LightColorComponents components alignas(16);
};

struct SpotLight
{
    vec3 posWorld                   alignas(16);
    vec3 dirWorld                   alignas(16);
    LightColorComponents components alignas(16);
    f32 cosInner                    alignas(4);
    f32 cosOuter                    alignas(4);
};

struct Environment
{
    std::array<PointLight, MAX_POINT_LIGHTS> pointLights                    alignas(16);
    u32 numPointLights                                                      alignas(4);
    std::array<DirectionalLight, MAX_DIRECTIONAL_LIGHTS> directionalLights  alignas(16);
    u32 numDirectionalLights                                                alignas(4);
    std::array<SpotLight, MAX_SPOT_LIGHTS> spotLights                       alignas(16);
    u32 numSpotLights                                                       alignas(4);
    u32 skyboxTexId                                                         alignas(4);
    u32 skyboxSamplerId                                                     alignas(4);
};