struct Light
{
    float4 Position; // w = type (0: directional, 1: point, 2: spot)
    float4 Direction;
    float4 Color;
    float4 Parameters; // x = innerCutoff (for spot light), y = outerCutoff (for spot light), z = isEnabled (1: enabled, 0: disabled)
};

float DistributionGGX(float3 N, float3 H, float roughness)
{
    const float PI = 3.14159265359;
    
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void reflectanceEquation(float3 V, float3 N, float3 F0, float3 albedo, float roughness, float metallic, float3 L, float3 radiance, out float3 diffuseOut, out float3 specularOut)
{
    const float PI = 3.14159265359;
    
    float3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    float3 specular = numerator / denominator;
    
    // kS is equal to Fresnel
    float3 kS = F;

    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;

    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    // float NdotL = max(dot(N, L), 0.0);
    // toon shading: quantize NdotL to create discrete lighting levels
    float NdotL = saturate(dot(N, L));
    float levels = 2.0f;
    float toon = floor(NdotL * levels) / (levels - 1.0f);

    diffuseOut = kD * albedo / PI * radiance * toon;
    specularOut = specular * radiance * toon;
}
// direction ------------------------------------------------------------------
void calculateDirectionLight(float3 V, float3 N, float3 F0, float3 albedo, float roughness, float metallic, Light light, out float3 diffuseOut, out float3 specularOut)
{
    // calculate per-light radiance
    float3 L = normalize(-light.Direction).xyz;
    float3 radiance = light.Color.xyz * light.Color.w;
    
    reflectanceEquation(V, N, F0, albedo, roughness, metallic, L, radiance, diffuseOut, specularOut);
}
// point ----------------------------------------------------------------------
void calculatePointLight(float3 lightPosition, float3 lightColor, float3 WorldPos, float3 V, float3 N, float3 F0, float3 albedo, float roughness, float metallic, out float3 diffuseOut, out float3 specularOut)
{
    float3 L = normalize(lightPosition - WorldPos);
    float distance = length(lightPosition - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = lightColor * attenuation;

    reflectanceEquation(V, N, F0, albedo, roughness, metallic, L, radiance, diffuseOut, specularOut);
}