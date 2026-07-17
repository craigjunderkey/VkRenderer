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

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    output.pos = mul(frame.projection,
        mul(frame.view,
            mul(obj.model, float4(input.inPosition, 1.0))
        )
    );

    output.normal = input.normal;

    // calculate tex coords
    float2 texOffset = onCreate.materials[obj.matId].texOffset;
    float2 texScale = onCreate.materials[obj.matId].texScale;
    output.fragTexCoord = texOffset + input.texCoord * texScale;

    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    int texId = onCreate.materials[obj.matId].texId;
    int samplerId = onCreate.materials[obj.matId].samplerId;
    return float4(
        onCreate.textures[texId].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb,
        1.0
    );
}
