struct VSInput
{
    float3 inPosition;
    float3 normal;
    float2 texCoord;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 normal;
    float2 fragTexCoord;
};

struct MVP
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};
ConstantBuffer<MVP> mvp;

Sampler2D texture;

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;
    
    output.pos = mul(mvp.projection,
        mul(mvp.view,
            mul(mvp.model, float4(input.inPosition, 1.0))
        )
    );

    output.normal = input.normal;
    output.fragTexCoord = float2(1.0, 1.25) + input.texCoord * float2(4.0, 4.0);

    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    return float4(texture.Sample(vertIn.fragTexCoord).rgb, 1.0);
}