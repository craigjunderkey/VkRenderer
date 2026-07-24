struct Material
{
    int diffMap;
    int specMap;
    int samplerId;
    float shininess;
    bool isUnlit;
    float2 texOffset;
    float2 texScale;
    float3 diffColor;
    float3 specColor;
};

#define UNKNOWN_SIZE 1

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
};
struct PerObject
{
    uint matId;
    float4x4 model;
};

ParameterBlock<OnCreate> onCreate;
ParameterBlock<PerFrame> frame;

[[vk::push_constant]] PerObject obj;
