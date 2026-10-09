#include "default_incoming_data.slang"

struct ParticleFrame
{
    ConstantBuffer<PerFrame> data;
    RWStructuredBuffer<Particle> currParticles;
    StructuredBuffer<Particle> prevParticles;
};

ParameterBlock<ParticleFrame> frame;

[shader("compute")]
[NumThreads(256, 1, 1)]
void mainCompute(uint3 threadId : SV_DispatchThreadID)
{
    uint i = threadId.x;
    frame.currParticles[i].posRadius = frame.prevParticles[i].posRadius + frame.prevParticles[i].velocity * frame.data.dt;
    frame.currParticles[i].velocity = frame.prevParticles[i].velocity;   
}