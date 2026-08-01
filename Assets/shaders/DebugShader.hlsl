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
    float4 vColor : COLOR0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float4 Color : COLOR0; // interpolated diffuse color
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT Output;
    float4 pos = float4(input.vPos, 1.0f);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    Output.Position = pos;
    Output.Color = input.vColor;
    return Output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return input.Color;
}
