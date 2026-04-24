#pragma pack_matrix(row_major)

cbuffer ViewProjectionConstantBuffer : register(b0)
{
    matrix View; // view matrix
    matrix Projection; // projection matrix
};

cbuffer ModelConstantBuffer : register(b1)
{
    matrix mWorld;
}

cbuffer SkinnedModelConstantBuffer : register(b2)
{
    matrix mBones[256];
}

struct VS_INPUT
{
    float3 vPos : POSITION;
    float3 vNormal : NORMAL;
    float2 vUV : TEXCOORD0;
    float4 vTanget : TANGENT;
    int4 vBoneIDs : BLENDINDICES;
    float4 vWeights : BLENDWEIGHT;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float2 UV : TEXCOORD0; // interpolated diffuse color
};

PS_INPUT VSMain(VS_INPUT input) // main is the default function name
{
    PS_INPUT Output;

    float4 totalPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        if (input.vBoneIDs[i] < 0)
            continue;

        matrix bone = mBones[input.vBoneIDs[i]];

        totalPosition += mul(float4(input.vPos, 1.0f), bone) * input.vWeights[i];
    }
  
    // totalPosition = mul(float4(input.vPos, 1.0f), mBones[0]);
    // totalPosition = float4(input.vPos, 1.0f);
    // Transform the position from object space to homogeneous projection space
    totalPosition = mul(totalPosition, mWorld);
    totalPosition = mul(totalPosition, View);
    totalPosition = mul(totalPosition, Projection);
    Output.Position = totalPosition;

    // Just pass through the color data
    Output.UV = input.vUV;

    return Output;
}

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, input.UV);
    if (color.a < 0.1f)
        discard;

    return color;
}