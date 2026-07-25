struct LightCalculationVars
{
    // user-specified
    // note: all vectors must be in view space

    float3 matDiff;
    float3 matSpec;
    float matShininess;
    float3 fragNormal;  // must arrive normalized
    float3 fragPos;
    float4x4 viewMatrix;

    // computed by finish
    float3 viewDir;

    [mutating]
    void finish()
    {
        this.viewDir = normalize(-this.fragPos); // dir from frag to eye
    }
};

interface ILightCaster
{
    float3 calculateContribution(LightCalculationVars data);
};

struct LightColorComponents
{
    float3 ambient;
    float3 diffuse;
    float3 specular;
};

struct PointLight : ILightCaster
{
    float3 position; // world space
    LightColorComponents components;

    float3 calculateContribution(LightCalculationVars data)
    {
        float3 position = mul(data.viewMatrix, float4(this.position, 1.0)).xyz;
        float3 color = float3(0.0, 0.0, 0.0);
        float3 lightDir = normalize(position - data.fragPos); // dir from frag to light

        // ambient
        color += this.components.ambient * data.matDiff;

        // diffuse
        float diffCoeff = max(0.0, dot(data.fragNormal, lightDir));
        color += diffCoeff * this.components.diffuse * data.matDiff;

        // specular
        float3 halfway = normalize(data.viewDir + lightDir);
        float specCoeff = pow(max(0.0, dot(halfway, data.fragNormal)), data.matShininess);
        color += specCoeff * this.components.specular * data.matSpec;

        return color;
    }
};