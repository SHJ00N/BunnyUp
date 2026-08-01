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

cbuffer MaterialConstantBuffer : register(b3)
{
    float4 mColor;
    float Roughness;
    float Metallic;
    uint HasNormal;
    uint HasRougness;
    uint HasMetallic;
    uint HasAmbientOcclusion;
    float Padding[2];
}

cbuffer SpriteSheetConstantBuffer : register(b6)
{
    float2 frameSize;
    float2 frameOffset;
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
    float2 UV : TEXCOORD0; // interpolated texture color
};


PS_INPUT VSMain(VS_INPUT input) // main is the default function name
{
    PS_INPUT Output;

    // world position
    float3 center = mWorld[3].xyz;

    // get view transform right / up vector
    float3 right = InvView[0].xyz;
    float3 up = InvView[1].xyz;
    
    float3 scale;
    scale.x = length(mWorld[0].xyz);
    scale.y = length(mWorld[1].xyz);
    scale.z = length(mWorld[2].xyz);
    
    // configure quad
    float3 worldPos = center + right * input.vPos.x * scale.x + up * input.vPos.y * scale.y;
    float4 viewPos = mul(float4(worldPos, 1.0f), View);
    Output.Position = mul(viewPos, Projection);
    
    Output.UV = input.vUV;
    
    return Output;
}

Texture2D billboardTexture : register(t0);
SamplerState linearSampler : register(s0);

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.UV * frameSize + frameOffset;
    float4 color = billboardTexture.Sample(linearSampler, uv);
    color *= mColor;
    return color;
}