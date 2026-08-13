struct VSInput
{
    float3 inPosition;
    float3 normal;
    float2 texCoord;
};

struct VSOutput
{
    float4 vertPos : SV_POSITION;
};

struct DebugLightDrawData
{
    float4x4 mvp;
    float4 color;
};

[[vk_push_constant]] DebugLightDrawData light;

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;
    output.vertPos = mul(light.mvp, float4(input.inPosition, 1.0));
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    return light.color;
}