#include "prelude.slang"

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
    int diffMap = onCreate.materials[obj.matId].diffMap;
    int samplerId = onCreate.materials[obj.matId].samplerId;
    float3 color;

    if (diffMap == NO_TEXTURE)
    {
        color = onCreate.materials[obj.matId].diffColor;
    }
    else
    {
        color = onCreate.textures[diffMap].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb;
    }

    return float4(color, 1.0);
}
