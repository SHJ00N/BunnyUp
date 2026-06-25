#pragma pack_matrix(row_major)

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
    float Roughness;
    float Metallic;
    float PaddingMat[2];
}

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

struct PS_OUTPUT
{
    float4 Position : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Albedo : SV_Target2;
    float4 MetallicRoughnessAO : SV_Target3;
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

PS_OUTPUT PSMain(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT Output;
    
    // store albedo, discard if alpha is too low
    float4 albedo = albedoMap.Sample(sampler0, input.UV);
    Output.Albedo = float4(albedo.rgb * mColor.rgb, albedo.a);
    if (albedo.a < 0.1f)
        discard;
    
    Output.Position = input.WorldPosition; // store world position in output for later use in deferred shading
    // store normal
    float3 normal = normalize(normalMap.Sample(sampler1, input.UV).rgb * 2.0f - 1.0f); // convert from [0, 1] to [-1, 1]
    normal = normalize(mul(normal, float3x3(input.Tangent, input.Bitangent, input.Normal))) * 0.5f + 0.5f;
    Output.Normal = float4(normal, 0.0f);
    
    Output.MetallicRoughnessAO = float4(Metallic, Roughness, 1.0f, 1.0f); // placeholder, can be replaced with actual metallic, roughness, ao values from texture maps
    
    return Output;
}