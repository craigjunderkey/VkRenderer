#include "light.slang"

#define UNKNOWN_SIZE 1
#define NO_TEXTURE -1
#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 64
#define MAX_SPOT_LIGHTS 16

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
};

struct OnCreate
{
    Material materials[UNKNOWN_SIZE];
    Texture2D textures[UNKNOWN_SIZE];
    SamplerState samplers[UNKNOWN_SIZE];
};

struct PerFrame
{
    float4x4 view;
    float4x4 projection;
    Environment env;
};

struct PerObject
{
    uint matId;
    float4x4 model;
    float3x3 normal;
};

ParameterBlock<OnCreate> onCreate;
ParameterBlock<PerFrame> frame;

[[vk::push_constant]] PerObject obj;