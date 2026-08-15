#include "prelude.slang"
#include "default_incoming_data.slang"

struct VSOutput
{
    float4 vertPos : SV_POSITION;
};

ParameterBlock<OnCreate> onCreate;
ParameterBlock<PerFrame> frame;

[shader("vertex")]
VSOutput vertMain(VSInput vertIn)
{
    VSOutput output;
    float3 pos = mul(float3x3(frame.view), vertIn.inPosition);
    output.vertPos = pos.xyzz;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    return frame.skybox.Sample(onCreate.samplers[0], vertIn.vertPos.xyz);
}