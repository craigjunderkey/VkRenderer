struct LightCalculationVars
{
    // user-specified
    // note: all vectors must be in view space

    float3 matDiff;
    float3 matSpec;
    float matShininess;
    float4 vertNormalView;
    float4 vertPosView;
    float4x4 viewMatrix;

    // computed by finish
    float4 viewDir;

    [mutating]
    void finish()
    {
        this.viewDir = normalize(-this.vertPosView); // dir from frag to eye
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
        float3 color = float3(0.0, 0.0, 0.0);

        // calculate helpful vectors
        float4 lightPosView = mul(data.viewMatrix, float4(this.position, 1.0)); // light pos: world -> view
        float4 lightDir = normalize(lightPosView - data.vertPosView);           // from surface to light

        // ambient term
        color += this.components.ambient * data.matDiff;

        // diffuse term
        float diffCoeff = max(0.0, dot(lightDir, data.vertNormalView));
        color += diffCoeff * this.components.diffuse * data.matDiff;

        // specular term
        float4 halfway = normalize(lightDir + data.viewDir);
        float hDotN = max(0.0, dot(halfway, data.vertNormalView));
        float specCoeff = pow(hDotN, data.matShininess);
        color += specCoeff * this.components.specular * data.matSpec;

        return color;
    }
};