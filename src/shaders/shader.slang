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
    float3 normal;
    //float3 fragNormal; // view space
    float3 fragPos; // view space
    float2 fragTexCoord;
};

LightCalculationVars cacheLightVars(VSOutput vertIn)
{
    LightCalculationVars lightVars;

    // store stuff we already have GRUMB
    lightVars.fragNormal = normalize(mul(obj.normal, vertIn.normal));
    lightVars.fragPos = vertIn.fragPos;
    lightVars.matShininess = onCreate.materials[obj.matId].shininess;

    // get material diffuse and specular colors
    int diffMap = onCreate.materials[obj.matId].diffMap;
    int specMap = onCreate.materials[obj.matId].specMap;
    uint samplerId = onCreate.materials[obj.matId].samplerId;

    if (diffMap == NO_TEXTURE)
    {
        lightVars.matDiff = onCreate.materials[obj.matId].diffColor;
    }
    else
    {
        lightVars.matDiff = onCreate.textures[diffMap].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb;
    }

    if (specMap == NO_TEXTURE)
    {
        lightVars.matSpec = onCreate.materials[obj.matId].specColor;
    }
    else
    {
        lightVars.matSpec = onCreate.textures[specMap].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb;
    }

    // finish and return
    lightVars.finish();
    return lightVars;
}

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    // calculate vert and frag positions
    output.fragPos = mul(frame.view, mul(obj.model, float4(input.inPosition, 1.0))).xyz;
    output.vertPos = mul(frame.projection, float4(output.fragPos, 1.0));

    // calculate frag normal
    output.normal = input.normal;

    // calculate tex coords
    float2 texOffset = onCreate.materials[obj.matId].texOffset;
    float2 texScale = onCreate.materials[obj.matId].texScale;
    output.fragTexCoord = texOffset + input.texCoord * texScale;

    return output;
}

// TODO: don't light unlit materials

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    float3 color = float3(0.0, 0.0, 0.0);
    LightCalculationVars lightVars = cacheLightVars(vertIn);

    for (uint i = 0; i < frame.env.numPointLights; i++)
    {
        color += frame.env.pointLights[i].calculateContribution(lightVars);
    }

    return float4(color, 1.0);
}
