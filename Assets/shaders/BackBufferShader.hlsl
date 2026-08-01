#pragma pack_matrix(row_major)

struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float2 UV : TEXCOORD0; // interpolated texture color
};


PS_INPUT VSMain(VS_INPUT input) // main is the default function name
{
    PS_INPUT Output;
    Output.Position = float4(input.vPos, 1.0f);
    Output.UV = input.vUV;
    return Output;
}

Texture2D screenTexture : register(t0);
SamplerState screenSampler : register(s0);

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 color = screenTexture.Sample(screenSampler, input.UV);
    return color;
}