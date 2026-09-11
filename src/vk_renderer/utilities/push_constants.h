#pragma once
#include "common.h"

struct ScenePushConstants
{
    u32 matId       alignas(4);
    mat4 model      alignas(16);
    mat4 normal     alignas(16);
    mat4 modelView  alignas(16);
};