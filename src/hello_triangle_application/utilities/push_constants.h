#pragma once
#include "common.h"

struct PushConstants
{
    u32 matId   alignas(4);
    mat4 model  alignas(16);
    mat4 normal alignas(16);
};