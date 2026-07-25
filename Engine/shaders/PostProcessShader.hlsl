#pragma pack_matrix(row_major)

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

Texture2D fullScreenQuad : register(t0);
Texture2D normalMap : register(t1);
Texture2D depthMap : register(t2);
SamplerState linearClamp : register(s0);
SamplerState pointClamp : register(s1);

// Calculate outline color with Sobel operator
float CalculateSobelFilter(float2 uv, float2 texelSize)
{
    // Sample the depth and normal maps in a 3x3 neighborhood around the current pixel
    float depth[3][3];
    float3 normal[3][3];
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            depth[i + 1][j + 1] = depthMap.Sample(pointClamp, uv + float2(j, i) * texelSize).r;
            normal[i + 1][j + 1] = normalMap.Sample(linearClamp, uv + float2(j, i) * texelSize).xyz;
        }
    }
    
    // Apply the Sobel operator to calculate the edge strength
    float depthX = depth[0][2] + 2.0f * depth[1][2] + depth[2][2] - (depth[0][0] + 2.0f * depth[1][0] + depth[2][0]);
    float depthY = depth[2][0] + 2.0f * depth[2][1] + depth[2][2] - (depth[0][0] + 2.0f * depth[0][1] + depth[0][2]);
    
    float depthEdge = abs(depthX) + abs(depthY);
    
    // Apply the Sobel operator to calculate the normal edge strength
    float3 normalX = normal[0][2] + 2.0f * normal[1][2] + normal[2][2] - (normal[0][0] + 2.0f * normal[1][0] + normal[2][0]);
    float3 normalY = normal[2][0] + 2.0f * normal[2][1] + normal[2][2] - (normal[0][0] + 2.0f * normal[0][1] + normal[0][2]);
   
    float normalEdge = length(normalX) + length(normalY);
    
    return depthEdge * 100.0f + normalEdge * 0.05f;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // Get texel size from the screen size
    float2 screenSize;
    fullScreenQuad.GetDimensions(screenSize.x, screenSize.y);
    float2 texelSize = 1.0f / screenSize;
    
    float3 sceneColor = fullScreenQuad.Sample(linearClamp, input.UV).xyz;
     // HDR tonemapping
    sceneColor = sceneColor / (sceneColor + 1.0f);
    // gamma correct
    sceneColor = pow(sceneColor, 1.0f / 2.2f);
    
    // apply Sobel filter to calculate edge strength
    float edgeStrength = CalculateSobelFilter(input.UV, texelSize);
    edgeStrength = smoothstep(0.05f, 0.25f, edgeStrength);
    float3 finalColor = lerp(sceneColor, float3(0.3f, 0.3f, 0.3f), edgeStrength);

    return float4(finalColor, 1.0f);
}