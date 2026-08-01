#pragma pack_matrix(row_major)

cbuffer TextConstantBuffer : register(b7)
{
    float2 textPosition;
    float2 textScale;
    float2 textUVMin;
    float2 textUVMax;
    float4 textColor;
}

cbuffer ScreenConstantBuffer : register(b8)
{
    uint screenWidth;
    uint screenHeight;
    float Padding[2];
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
    
    // local position of quad (-0.5 ~ 0.5)
    float2 localPosition;
    localPosition.x = input.vPos.x + 0.5f;
    localPosition.y = 0.5f - input.vPos.y;
    float2 pixelPosition = textPosition + localPosition * textScale;

    // Pixel ¡æ NDC
    float2 ndc;
    ndc.x = (pixelPosition.x / screenWidth) * 2.0f - 1.0f;
    ndc.y = 1.0f - (pixelPosition.y / screenHeight) * 2.0f;

    Output.Position = float4(ndc, 0.0f, 1.0f);

    // Quad UV ¡æ Atlas UV
    Output.UV = textUVMin + input.vUV * (textUVMax - textUVMin);

    return Output;
}

Texture2D fontAtlas : register(t0);
SamplerState fontSampler : register(s0);

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float alpha = fontAtlas.Sample(fontSampler, input.UV).r;

    return float4(textColor.rgb, textColor.a * alpha);
}