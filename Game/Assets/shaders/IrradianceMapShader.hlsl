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

TextureCube environmentMap : register(t0);
SamplerState linearClamp : register(s0);

static const float PI = 3.14159265359;

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float3 N = normalize(input.WorldPos);

    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    float sampleDelta = 0.25f;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            float3 sampleVec = normalize(tangentSample.x * right + tangentSample.y * up + tangentSample.z * N);
            sampleVec.z *= -1;

            irradiance += environmentMap.Sample(linearClamp, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    return float4(irradiance, 1.0);
}