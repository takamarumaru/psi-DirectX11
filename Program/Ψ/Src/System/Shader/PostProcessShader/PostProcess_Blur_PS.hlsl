#include "inc_PostProcess.hlsli"


Texture2D g_inputTex : register(t0);
SamplerState g_ss : register(s0);

cbuffer cb : register(b0)
{
    float4 g_offset[31];
};

float4 main(VSOutput In) : SV_Target0
{
    float3 color = 0;
    for (int i = 0; i < 31; i++)
    {
        color += g_inputTex.Sample(g_ss, In.UV + g_offset[i].xy).rgb * g_offset[i].z;
    }
    //color /= 31;

    return float4(color, 1);
}
