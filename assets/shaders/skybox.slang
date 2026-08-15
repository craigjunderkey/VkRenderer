#include "prelude.slang"
#include "default_incoming_data.slang"

/*
TODO: do something more intelligent
    e.g., set macros when compiling which reference SamplerName
find skybox textures at https://www.humus.name/index.php?page=Textures
*/
#define SKYBOX_SAMPLER_INDEX 2

struct VSOutput
{
    float4 vertPos : SV_POSITION;
    float3 texCoords;
};

ParameterBlock<OnCreate> onCreate;
ParameterBlock<PerFrame> frame;

[shader("vertex")]
VSOutput vertMain(VSInput vertIn)
{
    VSOutput output;
    output.texCoords = vertIn.inPosition;
    float4 pos = mul(frame.projection, float4(mul(float3x3(frame.view), vertIn.inPosition), 1.0));
    output.vertPos = pos.xyww;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    return frame.skybox.Sample(onCreate.samplers[SKYBOX_SAMPLER_INDEX], vertIn.texCoords);
}