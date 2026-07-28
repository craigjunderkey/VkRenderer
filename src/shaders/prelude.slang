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