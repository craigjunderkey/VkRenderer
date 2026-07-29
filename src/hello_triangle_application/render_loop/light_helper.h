#pragma once
#include "../utilities/common.h"

class LightHelper
{
public:
    static vec3 attenuationFactorsFromRadius(f32 radius);
};