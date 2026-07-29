// ---- prelude.slang

#define UNKNOWN_SIZE 1
#define NO_TEXTURE -1
#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 64
#define MAX_SPOT_LIGHTS 16

struct Material
{
    int diffMap;
    int specMap;
    uint samplerId;
    float shininess;
    bool isUnlit;
    float2 texOffset;
    float2 texScale;
    float3 diffColor;
    float3 specColor;
};

struct Environment
{
    PointLight pointLights[MAX_POINT_LIGHTS];
    uint numPointLights;
};


// ---- light.slang

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
    float3 worldPosition;           // world space
    LightColorComponents components;
    float3 attenuationFactors;      // constant (k_c), linear (k_l), quadratic (k_q)

    float3 calculateContribution(LightCalculationVars data)
    {
        float3 color = float3(0.0, 0.0, 0.0);

        // calculate helpful vectors
        float4 lightPosView = mul(data.viewMatrix, float4(this.worldPosition, 1.0));    // light pos: world -> view
        float4 lightDir = normalize(lightPosView - data.vertPosView);                   // from surface to light

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

        // attenuate with distance
        float distance = length(data.vertPosView - lightPosView);
        float attenuation = 1.0 / dot(this.attenuationFactors, float3(1.0, distance, distance * distance));
        color *= attenuation;

        return color;
    }
};


// ---- shader.slang

struct VSInput
{
    float3 inPosition;
    float3 normal;
    float2 texCoord;
};

struct VSOutput
{
    float4 vertPos : SV_POSITION;
    float4 vertNormalLocal;     // local space
    float4 vertPosView;         // view space
    float2 fragTexCoord;
};

struct OnCreate
{
    Material materials[UNKNOWN_SIZE];
    Texture2D textures[UNKNOWN_SIZE];
    SamplerState samplers[UNKNOWN_SIZE];
};

struct PerFrame
{
    float4x4 view;
    float4x4 projection;
    Environment env;
};

struct PerObject
{
    uint matId;
    float4x4 model;
    float4x4 normal;
};

ParameterBlock<OnCreate> onCreate;
ParameterBlock<PerFrame> frame;

[[vk::push_constant]] PerObject obj;

LightCalculationVars cacheLightCalculationVars(VSOutput vertIn)
{
    LightCalculationVars data;
    data.vertPosView = vertIn.vertPosView;
    data.vertNormalView = float4(normalize(mul(float3x3(obj.normal), normalize(vertIn.vertNormalLocal).xyz)), 0.0);
    data.viewMatrix = frame.view;
    data.matShininess = onCreate.materials[obj.matId].shininess;

    // get material diffuse and specular colors
    int diffMap = onCreate.materials[obj.matId].diffMap;
    int specMap = onCreate.materials[obj.matId].specMap;
    uint samplerId = onCreate.materials[obj.matId].samplerId;

    if (diffMap == NO_TEXTURE)
        data.matDiff = onCreate.materials[obj.matId].diffColor;
    else
        data.matDiff = onCreate.textures[diffMap].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb;

    if (specMap == NO_TEXTURE)
        data.matSpec = onCreate.materials[obj.matId].specColor;
    else
        data.matSpec = onCreate.textures[specMap].Sample(onCreate.samplers[samplerId], vertIn.fragTexCoord).rgb;

    return data;
}

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;

    // calculate positions
    output.vertPos = mul(frame.projection, mul(frame.view, mul(obj.model, float4(input.inPosition, 1.0))));
    output.vertPosView = mul(frame.view, mul(obj.model, float4(input.inPosition, 1.0)));

    // calculate normal
    output.vertNormalLocal = float4(input.normal, 0.0);

    // calculate tex coords
    float2 texOffset = onCreate.materials[obj.matId].texOffset;
    float2 texScale = onCreate.materials[obj.matId].texScale;
    output.fragTexCoord = texOffset + input.texCoord * texScale;

    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target
{
    float3 color = float3(0.0, 0.0, 0.0);

    LightCalculationVars data = cacheLightCalculationVars(vertIn);
    data.finish();

    // get color contribution of all the lights
    for (uint i = 0; i < frame.env.numPointLights; i++)
    {
        color += frame.env.pointLights[i].calculateContribution(data);
    }

    return float4(color, 1.0);
}
