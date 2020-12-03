#include "inc_PostProcess.hlsli"

cbuffer cb : register(b0)
{
    float4  g_color;    //描画色
    int     g_mode;     //描画モード
};



Texture2D g_inputTex : register(t0);
SamplerState g_ss : register(s0);

float4 main(VSOutput In) : SV_Target0
{
    float4 texColor = g_inputTex.Sample(g_ss, In.UV);

    switch (g_mode)
    {
    //グレースケール
    case 1:
        texColor.x = (texColor.x + texColor.y + texColor.z) / 3;
        texColor.y = texColor.x;
        texColor.z = texColor.x;
        break;
    //セピア調
    case 2:
        float colorPercentage = (texColor.x + texColor.y + texColor.z) / 3.0f;
        texColor.x = (240 / 225.0f) * (colorPercentage / 1.0f);
        texColor.y = (200 / 225.0f) * (colorPercentage / 1.0f);
        texColor.z = (145 / 225.0f) * (colorPercentage / 1.0f);
        break;
    }

    return texColor * g_color;
}
