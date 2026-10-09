#pragma once
#include "prelude.slang"

struct VSInput
{
    float3 inPosition;
    float3 normal;
    float2 texCoord;
};

struct OnCreate
{
    Material materials[NUM_MATERIALS];
    Texture2D textures[NUM_TEXTURES];
    TextureCube textureCubes[NUM_TEXCUBES];
    SamplerState samplers[NUM_SAMPLERS];
};

struct PerFrame
{
    float4x4 view;
    float4x4 projection;
    Environment env;
    float dt;
    RWStructuredBuffer<Particle> currParticles;
    StructuredBuffer<Particle> prevParticles;
};