#pragma pack_matrix(row_major)

cbuffer ViewProjectionConstantBuffer : register(b0)
{
    float4 CameraPosition;
    matrix View; // view matrix
    matrix Projection; // projection matrix
    matrix InvView;
    matrix InvProjection; // inverse projection matrix
};

struct VS_INPUT
{
    float3 vPos : POSITION;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float3 WorldPos : TEXCOORD0;
};

PS_INPUT VSMain(VS_INPUT input) // main is the default function name
{
    PS_INPUT Output;

    Output.WorldPos = input.vPos;
    Output.Position = mul(mul(float4(input.vPos, 1.0f), View), Projection);

    return Output;
}

Texture2D equirectangularMap : register(t0);
SamplerState linearClamp : register(s0);

static const float2 invAtan = float2(0.1591, 0.3183);
float2 SamplerSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float2 uv = SamplerSphericalMap(normalize(input.WorldPos));
    float3 color = equirectangularMap.Sample(linearClamp, uv).rgb;

    return float4(color, 1.0);
}