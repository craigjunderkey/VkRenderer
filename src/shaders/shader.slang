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
struct Material
{
    int texId;
    int samplerId;
    float shininess;
    bool isUnlit;
    float2 texOffset;
    float2 texScale;
    float3 ambient;
    float3 diffuse;
    float3 specular;
};

struct OnCreate
{
    Material materials[2];
    Texture2D textures[2];
    SamplerState samplers[1];
};
struct EveryFrame
{
    ConstantBuffer<MVP> mvp;
    Sampler2D texture;
};

ParameterBlock<OnCreate> onCreate;
ParameterBlock<EveryFrame> everyFrame;

[[vk::push_constant]] uint matId;

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    output.pos = mul(everyFrame.mvp.projection,
        mul(everyFrame.mvp.view,
            mul(everyFrame.mvp.model, float4(input.inPosition, 1.0))
        )
    );

    output.normal = input.normal;

    // calculate tex coords
    float2 texOffset = onCreate.materials[matId].texOffset;
    float2 texScale = onCreate.materials[matId].texScale;
    output.fragTexCoord = texOffset + input.texCoord * texScale;

    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    int texId = onCreate.materials[matId].texId;
    int samplerId = onCreate.materials[matId].samplerId;
    return float4(
        onCreate.textures[texId].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb,
        1.0
    );
}
