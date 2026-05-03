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

cbuffer MaterialConstantBuffer : register(b3)
{
    float4 mColor;
}

struct VS_INPUT
{
    float3 vPos : POSITION;
    float3 vNormal : NORMAL;
    float2 vUV : TEXCOORD0;
    float4 vTangent : TANGENT;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float4 Color : COLOR0; // interpolated diffuse color
};


PS_INPUT VSMain(VS_INPUT input) // main is the default function name
{
    PS_INPUT Output;

    float4 pos = float4(input.vPos, 1.0f);

    // Transform the position from object space to homogeneous projection space
    pos = mul(pos, mWorld);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    Output.Position = pos;

    // Just pass through the color data
    Output.Color = mColor;

    return Output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // Just pass through the color data
    return input.Color;
}