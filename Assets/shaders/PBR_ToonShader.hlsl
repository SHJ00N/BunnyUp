#pragma pack_matrix(row_major)
#include "Common/Lighting.hlsli"

cbuffer ViewProjectionConstantBuffer : register(b0)
{
    float4 CameraPosition;
    matrix View; // view matrix
    matrix Projection; // projection matrix
    matrix InvView;
    matrix InvProjection; // inverse projection matrix
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
SamplerState pointClamp : register(s1);

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float depth = depthMap.Sample(pointClamp, input.UV).r;
    if(depth >= 1.0f) // if depth is 1.0, it means this pixel is background, discard it for better performance
        discard;
    
    float4 sampledAlbedo = albedoMap.Sample(linearClamp, input.UV);
    float3 albedo = sampledAlbedo.rgb;
    float alpha = sampledAlbedo.a;
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
    
    float4 FragColor = float4(color, alpha);
    
    return FragColor;
}