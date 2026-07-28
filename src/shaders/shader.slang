#include "prelude.slang"

struct VSInput
{
    float3 inPosition;
    float3 normal;
    float2 texCoord;
};

struct VSOutput
{
    float4 vertPos : SV_POSITION;
    float4 vertNormalLocal;  // local space
    float4 vertPosView;     // view space
    float2 fragTexCoord;
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
    float4x4 normal;
};

ParameterBlock<OnCreate> onCreate;
ParameterBlock<PerFrame> frame;

[[vk::push_constant]] PerObject obj;

LightCalculationVars cacheLightCalculationVars(VSOutput vertIn)
{
    LightCalculationVars data;
    data.vertPosView = vertIn.vertPosView;
    data.vertNormalView = float4(normalize(mul(float3x3(obj.normal), normalize(vertIn.vertNormalLocal).xyz)), 0.0);
    data.viewMatrix = frame.view;
    data.matShininess = onCreate.materials[obj.matId].shininess;

    // get material diffuse and specular colors
    int diffMap = onCreate.materials[obj.matId].diffMap;
    int specMap = onCreate.materials[obj.matId].specMap;
    uint samplerId = onCreate.materials[obj.matId].samplerId;

    if (diffMap == NO_TEXTURE)
        data.matDiff = onCreate.materials[obj.matId].diffColor;
    else
        data.matDiff = onCreate.textures[diffMap].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb;

    if (specMap == NO_TEXTURE)
        data.matSpec = onCreate.materials[obj.matId].specColor;
    else
        data.matSpec = onCreate.textures[specMap].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb;

    return data;
}

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    // calculate positions
    output.vertPos = mul(frame.projection, mul(frame.view, mul(obj.model, float4(input.inPosition, 1.0))));
    output.vertPosView = mul(frame.view, mul(obj.model, float4(input.inPosition, 1.0)));

    // calculate normal
    output.vertNormalLocal = float4(input.normal, 0.0);

    // calculate tex coords
    float2 texOffset = onCreate.materials[obj.matId].texOffset;
    float2 texScale = onCreate.materials[obj.matId].texScale;
    output.fragTexCoord = texOffset + input.texCoord * texScale;

    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    float3 color = float3(0.0, 0.0, 0.0);

    LightCalculationVars data = cacheLightCalculationVars(vertIn);
    data.finish();

    // get color contribution of all the lights
    for (uint i = 0; i < frame.env.numPointLights; i++)
    {
        color += frame.env.pointLights[i].calculateContribution(data);
    }

    return float4(color, 1.0);
}
