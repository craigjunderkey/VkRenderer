#pragma once

#define UNKNOWN_SIZE 1
#define NO_TEXTURE -1
#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 64
#define MAX_SPOT_LIGHTS 16

#include "light.slang"

struct Material
{
    int diffMap;
    int specMap;
    uint samplerId;
    float shininess;
    bool isUnlit;
    float2 texOffset;
    float2 texScale;
    float3 diffColor;
    float3 specColor;
};

struct Environment
{
    PointLight pointLights[MAX_POINT_LIGHTS];
    uint numPointLights;
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    uint numDirectionalLights;
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    uint numSpotLights;

    uint skyboxTexId;
    uint skyboxSamplerId;
};

/*
    struct Particle
    {
        vec4 posRadius = { 0.0f, 0.0f, 0.0f, 1.0f };
        vec4 velocity = { 0.0f, 0.0f, 0.0f, 0.0f };
        u16 neighborCount = 0;
        f32 phase;
        f32 waterAmount = 0.0f;
        f32 temperature;
    };
*/

struct Particle
{
    float4 posRadius;
    float4 velocity;
    uint16_t neighborCount;
    float phase;
    float waterAmount;
    float temperature;
};