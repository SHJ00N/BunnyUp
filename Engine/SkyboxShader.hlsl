#pragma pack_matrix(row_major)

cbuffer ViewProjectionConstantBuffer : register(b0)
{
    float4 CameraPosition;
    matrix View; // view matrix
    matrix Projection; // projection matrix
    matrix InvView;
    matrix InvProjection;   // inverse projection matrix
};

struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float2 UV : TEXCOORD0; // interpolated diffuse color
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT Output;
    Output.UV = input.vUV;
    Output.Position = float4(input.vPos, 1.0f);
    
    return Output;
}

TextureCube environmentMap : register(t0);
Texture2D lightingMap : register(t1);
Texture2D depthMap : register(t2);
SamplerState linearClamp : register(s0);
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float depth = depthMap.Sample(linearClamp, input.UV).r;
    float3 color = lightingMap.Sample(linearClamp, input.UV).rgb;
    if(depth < 0.9999f)
    {
        return float4(color, 1.0f);
    }
    
    float2 ndc = input.UV * 2.0f - 1.0f; // -1 ~ 1
    float4 clip = float4(ndc.x, -ndc.y, 1.0f, 1.0f);    // flip axis y

    float4 viewRay = mul(clip, InvProjection);
    float3 viewDir = normalize(viewRay.xyz / viewRay.w);
    
    float3 dir = normalize(mul(float4(viewDir, 0.0f), InvView).xyz);
    // Sample the environment map using the world position as the direction
    float3 envColor = environmentMap.SampleLevel(linearClamp, dir, 0.0).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + float3(1.0f, 1.0f, 1.0f));
    envColor = pow(envColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    
    return float4(envColor, 1.0f);
}