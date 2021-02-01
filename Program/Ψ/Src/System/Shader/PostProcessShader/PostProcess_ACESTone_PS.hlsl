#include "inc_PostProcess.hlsli"

Texture2D g_inputTex : register(t0);
SamplerState g_ss : register(s0);

float3 ACESFilmicTone(float3 col)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;

    return saturate((col * (a * col + b)) / (col * (c * col + d) + e));
}

float4 main(VSOutput In) : SV_Target0
{
    float4 texColor = g_inputTex.Sample(g_ss, In.UV);

    texColor.xyz = ACESFilmicTone(texColor.xyz * 0.6);

    return texColor;
}
