#include "default_incoming_data.slang"

struct ParticleFrame
{
    ConstantBuffer<PerFrame> data;
    RWStructuredBuffer<Particle> currParticles;
    StructuredBuffer<Particle> prevParticles;
};

struct ParticleVertIn
{
    float4 posRadius;
};

struct ParticleVertOut
{
    float4 position : SV_Position;
    float radius : SV_PointSize;
};

struct ParticleFragIn
{
    float4 position : SV_Position;
}

ParameterBlock<ParticleFrame> frame;

[shader("compute")]
[NumThreads(256, 1, 1)]
void mainCompute(uint3 threadId : SV_DispatchThreadID)
{
    uint i = threadId.x;
    frame.currParticles[i].posRadius = frame.prevParticles[i].posRadius + frame.prevParticles[i].velocity * frame.data.dt;
    frame.currParticles[i].velocity = frame.prevParticles[i].velocity;   
}

[shader("vertex")]
ParticleVertOut mainVertex(ParticleVertIn in)
{
    ParticleVertOut out;
    out.position = mul(frame.data.projection, mul(frame.data.view, float4(in.posRadius.xyz, 1.0)));
    out.radius = in.posRadius.w;

    return out;
}

[shader("fragment")]
float4 mainFragment(ParticleFragIn in) : SV_Target
{
    return float4(1.0, 1.0, 1.0, 1.0);
}