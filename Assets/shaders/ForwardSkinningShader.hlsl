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

cbuffer ModelConstantBuffer : register(b1)
{
    matrix mWorld;
    matrix mNormalMatrix;
}

cbuffer SkinnedModelConstantBuffer : register(b2)
{
    matrix mBones[256];
}

cbuffer MaterialConstantBuffer : register(b3)
{
    float4 mColor;
    float mRoughness;
    float Metallic;
    uint HasNormal;
    uint HasRougness;
    uint HasMetallic;
    uint HasAmbientOcclusion;
    float PaddingMat[2];
}

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
    float3 vNormal : NORMAL;
    float2 vUV : TEXCOORD0;
    float4 vTangent : TANGENT;
    int4 vBoneIDs : BLENDINDICES;
    float4 vWeights : BLENDWEIGHT;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float2 UV : TEXCOORD0; // interpolated diffuse color
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 Bitangent : TEXCOORD3;
    float4 WorldPosition : TEXCOORD4;
};

PS_INPUT VSMain(VS_INPUT input) // main is the default function name
{
    PS_INPUT Output;

    float4 totalPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 totalNormal = float3(0.0f, 0.0f, 0.0f);
    float3 totalTangent = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        if (input.vBoneIDs[i] < 0)
            continue;

        matrix bone = mBones[input.vBoneIDs[i]];

        totalPosition += mul(float4(input.vPos, 1.0f), bone) * input.vWeights[i];
        totalNormal += mul(float4(input.vNormal, 0.0f), bone).xyz * input.vWeights[i];
        totalTangent += mul(float4(input.vTangent.xyz, 0.0f), bone).xyz * input.vWeights[i];
    }
    
    // position
    totalPosition = mul(totalPosition, mWorld);
    Output.WorldPosition = totalPosition; // store world position for later use in pixel shader
    
    totalPosition = mul(totalPosition, View);
    totalPosition = mul(totalPosition, Projection);
    Output.Position = totalPosition;

    // texture coordinate
    Output.UV = input.vUV;

    // normal, tangent, bitanget
    float3 N = mul(totalNormal, (float3x3) mNormalMatrix);
    float3 T = mul(totalTangent, (float3x3) mNormalMatrix);
    
    N = normalize(N);
    T = normalize(T - dot(T, N) * N);
    float3 B = cross(N, T) * input.vTangent.w;
    
    Output.Normal = N;
    Output.Tangent = T;
    Output.Bitangent = B;
    
    return Output;
}

Texture2D albedoMap : register(t0);
SamplerState sampler0 : register(s0);
Texture2D normalMap : register(t1);
SamplerState sampler1 : register(s1);
Texture2D roughnessMap : register(t2);
SamplerState sampler2 : register(s2);
Texture2D metallicMap : register(t3);
SamplerState sampler3 : register(s3);
Texture2D ambientOcclusionMap : register(t4);
SamplerState sampler4 : register(s4);
Texture2D opacityMap : register(t5);
SamplerState sampler5 : register(s5);

TextureCube irradianceMap : register(t10);
TextureCube prefilteredMap : register(t11);
Texture2D brdfLUT : register(t12);
SamplerState linearClamp : register(s10);

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // lighting value
    float3 albedo = albedoMap.Sample(sampler0, input.UV).rgb;
    float alpha = opacityMap.Sample(sampler5, input.UV).a;
    float metallic = HasMetallic == 1 ? metallicMap.Sample(sampler3, input.UV).r : Metallic;
    float roughness = HasRougness == 1 ? clamp(roughnessMap.Sample(sampler2, input.UV).r, 0.04f, 1.0f) : mRoughness;
    float ao = HasAmbientOcclusion == 1 ? ambientOcclusionMap.Sample(sampler4, input.UV).r : 1.0f;
    
    float3 worldPos = input.WorldPosition.xyz;
    float3 normal;
    if (HasNormal == 1)
    {
        normal = normalize(normalMap.Sample(sampler1, input.UV).rgb * 2.0f - 1.0f); // convert from [0, 1] to [-1, 1]
        normal = normalize(mul(normal, float3x3(input.Tangent, input.Bitangent, input.Normal)));
    }
    else
    {
        normal = normalize(float3(input.Normal));
    }
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