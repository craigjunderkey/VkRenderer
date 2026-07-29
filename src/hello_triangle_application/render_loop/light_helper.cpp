#include "light_helper.h"

struct AttenuationRow
{
    f32 distance;
    f32 constant;
    f32 linear;
    f32 quadratic;
};

// values from OGRE wiki (https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation)
constexpr std::array<AttenuationRow, 12> ATTENUATION_TABLE({
    { 7.0f, 1.0f, 0.7f, 1.8f },
    { 13.0f, 1.0f, 0.35f, 0.44f },
    { 20.0f, 1.0f, 0.22f, 0.20f },
    { 32.0f, 1.0f, 0.14f, 0.07f },
    { 50.0f, 1.0f, 0.09f, 0.032f },
    { 65.0f, 1.0f, 0.07f, 0.017f },
    { 100.0f, 1.0f, 0.045f, 0.0075f },
    { 160.0f, 1.0f, 0.027f, 0.0028f },
    { 200.0f, 1.0f, 0.022f, 0.0019f },
    { 325.0f, 1.0f, 0.014f, 0.0007f },
    { 600.0f, 1.0f, 0.007f, 0.0002f },
    { 3250.0f, 1.0f, 0.0014f, 0.000007f }
});

vec3 LightHelper::attenuationFactorsFromRadius(f32 radius)
{
    // clamp radius to only table values
    radius = glm::clamp(radius, 7.0f, 3250.0f - glm::epsilon<f32>());
    
    // find our spot in the table
    u32 i = 0;
    u32 min, max;
    while (i < ATTENUATION_TABLE.size() - 1)
    {
        if (radius < ATTENUATION_TABLE[i + 1].distance)
        {
            min = i;
            max = i + 1;
            break;
        }
        i++;
    }

    // compute blend factor
    f32 range = ATTENUATION_TABLE[max].distance - ATTENUATION_TABLE[min].distance;
    f32 blend = (radius - ATTENUATION_TABLE[min].distance) / range;
    
    // compute attenuation factors
    vec3 factors {
        (1.0f - blend) * ATTENUATION_TABLE[min].constant + blend * ATTENUATION_TABLE[max].constant,
        (1.0f - blend) * ATTENUATION_TABLE[min].linear + blend * ATTENUATION_TABLE[max].linear,
        (1.0f - blend) * ATTENUATION_TABLE[min].quadratic + blend * ATTENUATION_TABLE[max].quadratic
    };
    return factors;
}