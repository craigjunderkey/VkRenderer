#include "default_incoming_data.slang"

ParameterBlock<PerFrame> frame;

[shader("compute")]
[NumThreads(256, 1, 1)]
void mainCompute(uint3 threadId : SV_DispatchThreadID)
{
    uint i = threadId.x;
    frame.currParticles[i].posRadius = frame.prevParticles[i].posRadius + frame.prevParticles[i].velocity * frame.dt;
    frame.currParticles[i].velocity = frame.prevParticles[i].velocity;   
}