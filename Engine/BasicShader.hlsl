cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix mWorld; // world matrix for object
    matrix View; // view matrix
    matrix Projection; // projection matrix
};

struct VS_INPUT
{
    float3 vPos : POSITION;
    float3 vColor : COLOR0;
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
    Output.Color = float4(input.vColor, 1.0f);

    return Output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // Just pass through the color data
    return input.Color;
}