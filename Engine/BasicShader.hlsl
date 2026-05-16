#pragma pack_matrix(row_major)

cbuffer ViewProjectionConstantBuffer : register(b0)
{
    float4 CameraPosition;
    matrix View; // view matrix
    matrix Projection; // projection matrix
};

cbuffer ModelConstantBuffer : register(b1)
{
    matrix mWorld;
    matrix mNormalMatrix;
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
    float3 Normal : TEXCOORD1;
    float4 Color : COLOR0; // interpolated diffuse color
    float4 WorldPosition : TEXCOORD2;
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

    float4 pos = float4(input.vPos, 1.0f);

    // Transform the position from object space to homogeneous projection space
    pos = mul(pos, mWorld);
    Output.WorldPosition = pos;
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    Output.Position = pos;

    // Just pass through the color data
    Output.Color = mColor;
    
    Output.Normal = mul(input.vNormal, (float3x3) mNormalMatrix);

    return Output;
}

PS_OUTPUT PSMain(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT Output;
       // store albedo, discard if alpha is too low
    Output.Albedo = input.Color;
    if (Output.Albedo.a < 0.1f)
        discard;
    
    Output.Position = input.WorldPosition; // store world position in output for later use in deferred shading
    // store normal
    Output.Normal = float4(normalize(input.Normal) * 0.5f + 0.5f, 0.0f);
    
    Output.MetallicRoughnessAO = float4(0.0f, 0.0f, 1.0f, 1.0f); // placeholder, can be replaced with actual metallic, roughness, ao values from texture maps
    
    return Output;
}