#pragma once

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
    float3 posWorld;           // world space
    LightColorComponents components;
    float3 attenuationFactors;      // constant (k_c), linear (k_l), quadratic (k_q)

    float3 calculateContribution(LightCalculationVars data)
    {
        float3 color = float3(0.0, 0.0, 0.0);

        // calculate helpful vectors
        float4 lightPosView = mul(data.viewMatrix, float4(this.posWorld, 1.0));    // light pos: world -> view
        float4 lightDir = normalize(lightPosView - data.vertPosView);              // from surface to light

        // ambient term
        color += this.components.ambient * data.matDiff;

        // diffuse term
        float diffCoeff = max(0.0, dot(lightDir, data.vertNormalView));
        color += diffCoeff * this.components.diffuse * data.matDiff;

        // specular term
        float4 halfway = normalize(lightDir + data.viewDir);
        float specCoeff = pow(max(0.0, dot(halfway, data.vertNormalView)), data.matShininess);
        color += specCoeff * this.components.specular * data.matSpec;

        // attenuate with distance
        float distance = length(data.vertPosView - lightPosView);
        float attenuation = 1.0 / dot(this.attenuationFactors, float3(1.0, distance, distance * distance));
        color *= attenuation;

        return color;
    }
};

struct DirectionalLight : ILightCaster
{
    float3 dirWorld; // world space
    LightColorComponents components;

    float3 calculateContribution(LightCalculationVars data)
    {
        float3 color = float3(0.0, 0.0, 0.0);
        float4 lightDirView = normalize(mul(data.viewMatrix, float4(this.dirWorld, 0.0)));

        // ambient term
        color += this.components.ambient * data.matDiff;

        // diffuse term
        float diffCoeff = max(0.0, dot(lightDirView, data.vertNormalView));
        color += diffCoeff * this.components.diffuse * data.matDiff;

        // specular term
        float4 halfway = normalize(lightDirView + data.viewDir);
        float specCoeff = pow(max(0.0, dot(halfway, data.vertNormalView)), data.matShininess);
        color += specCoeff * this.components.specular * data.matSpec;

        return color;
    }
};

struct SpotLight : ILightCaster
{
    float3 posWorld; // world space
    float3 dirWorld; // world space
    LightColorComponents components;
    float cosInner;
    float cosOuter;

    float3 calculateContribution(LightCalculationVars data)
    {
        float3 color = float3(0.0, 0.0, 0.0);

        // ambient term
        color += this.components.ambient * data.matDiff;

        // get angle between light direction and light-to-frag vector
        float4 lightDirView = normalize(mul(data.viewMatrix, float4(dirWorld, 0.0)));
        float4 fragDirView = normalize(data.vertPosView - mul(data.viewMatrix, float4(this.posWorld, 1.0)));
        float cosAngle = dot(lightDirView, fragDirView);

        // compute attenuation
        float attenuation;
        if (cosAngle >= this.cosInner)
            attenuation = 1.0;
        else if (cosAngle <= this.cosOuter)
            return color; // attenuation = 0.0 (so, only ambient light)
        else
            attenuation = (cosAngle - this.cosOuter) / (this.cosInner - this.cosOuter);

        // diffuse term
        float diffCoeff = max(0.0, dot(data.vertNormalView, -fragDirView));
        color += attenuation * diffCoeff * this.components.diffuse * data.matDiff;

        // specular term
        float4 halfway = normalize(data.viewDir - fragDirView); // dir from frag to eye + dir from frag to light
        float specCoeff = pow(max(0.0, dot(halfway, data.vertNormalView)), data.matShininess);
        color += attenuation * specCoeff * this.components.specular * data.matSpec;

        return attenuation * color;
    }
};