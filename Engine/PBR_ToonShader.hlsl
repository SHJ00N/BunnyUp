#pragma pack_matrix(row_major)

cbuffer ViewProjectionConstantBuffer : register(b0)
{
    float4 CameraPosition;
    matrix View; // view matrix
    matrix Projection; // projection matrix
    matrix InvView;
    matrix InvProjection; // inverse projection matrix
};

struct Light
{
    float4 Position; // w = type (0: directional, 1: point, 2: spot)
    float4 Direction;
    float4 Color;
    float4 Parameters; // x = innerCutoff (for spot light), y = outerCutoff (for spot light), z = isEnabled (1: enabled, 0: disabled)
};

cbuffer LightConstantBuffer : register(b4)
{
    Light Lights[256];
    int NumLights;
    float PaddingLight[3];
};

cbuffer PrefilteredEnvMapConstantBuffer : register(b5)
{
    float Roughness;
    float Resolution;
    int MaxMip;
    float PaddingPrefiltered;
};

struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float2 UV : TEXCOORD0; // interpolated diffuse color
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT Output;
    Output.UV = input.vUV;
    Output.Position = float4(input.vPos, 1.0f);
    
    return Output;
}

Texture2D positionMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D albedoMap : register(t2);
Texture2D metallicRoughnessAoMap : register(t3);
Texture2D depthMap : register(t4);
TextureCube irradianceMap : register(t5);
TextureCube prefilteredMap : register(t6);
Texture2D brdfLUT : register(t7);
SamplerState linearClamp : register(s0);

static const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(float3 N, float3 H, float roughness)
{
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

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float depth = depthMap.Sample(linearClamp, input.UV).r;
    if(depth >= 1.0f) // if depth is 1.0, it means this pixel is background, discard it for better performance
        discard;
    
    float3 albedo = albedoMap.Sample(linearClamp, input.UV).rgb;
    float metallic = metallicRoughnessAoMap.Sample(linearClamp, input.UV).r;
    float roughness = clamp(metallicRoughnessAoMap.Sample(linearClamp, input.UV).g, 0.04f, 1.0f);
    float ao = metallicRoughnessAoMap.Sample(linearClamp, input.UV).b;
    
    // get world position from position map
    float3 worldPos = positionMap.Sample(linearClamp, input.UV).rgb;
    // get normal from normal map
    float3 normal = normalize(normalMap.Sample(linearClamp, input.UV).xyz * 2.0f - 1.0f);
    float3 viewDir = normalize(CameraPosition.xyz - worldPos);
    float3 reflection = reflect(-viewDir, normal);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallic);
 
    float3 diffuseSum = float3(0.0f, 0.0f, 0.0f);
    float3 specularSum = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < NumLights; i++)
    {
        Light light = Lights[i];
        float3 diffuse;
        float3 specular;
        if (light.Parameters.z < 0.5) // if light is disabled, skip
            continue;
        if (light.Position.w < 0.5) // directional light
        {
            calculateDirectionLight(viewDir, normal, F0, albedo, roughness, metallic, light, diffuse, specular);
        }
        else if (light.Position.w < 1.5) // point light
        {
            calculatePointLight(light.Position.xyz, light.Color.xyz * light.Color.w, worldPos, viewDir, normal, F0, albedo, roughness, metallic, diffuse, specular);
        }
        // spot light can be implemented similarly by adding cutoff calculations
        
        // toon specular
        float specStrength = max(specular.r, max(specular.g, specular.b));
        float specMask = floor(specStrength * 2.0) / 1.0;
        float3 toonSpecular = specular * specMask;
        
        diffuseSum += diffuse;
        specularSum += toonSpecular;
    }
    
    // ambient lighting with IBL
    float3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
    
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    float3 irradiance = irradianceMap.Sample(linearClamp, normal).rgb;
    float3 diffuse = irradiance * albedo;
    
    float3 prefilteredColor = prefilteredMap.SampleLevel(linearClamp, reflection, roughness * MaxMip).rgb;
    float2 brdf = brdfLUT.Sample(linearClamp, float2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    float3 ambient = (kD * diffuse + specular) * ao;
    
    float3 color = ambient + diffuseSum + specularSum;
    
    // HDR tonemapping
    color = color / (color + 1.0f);
    // gamma correct
    color = pow(color, 1.0f / 2.2f);

    float4 FragColor = float4(color, 1.0f);
    
    return FragColor;
}