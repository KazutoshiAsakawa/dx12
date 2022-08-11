#include "FBX.hlsli"
// 0番スロットに設定されたテクスチャ
Texture2D<float4> tex : register(t0);
// 0番スロットに設定されたサンプラー
SamplerState smp : register(s0);

struct PSOutput {
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};

//PSOutput main(VSOutput input) //: SV_TARGET
//{
//	PSOutput output;
//
//	// テクスチャマッピング
//	float4 texcolor = tex.Sample(smp, input.uv);
//	// Lambert反射
//	float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
//	float diffuse = saturate(dot(-light, input.normal));
//	float brightness = diffuse + 0.3f;
//	float4 shadecolor = float4(brightness, brightness, brightness, 1.0f);
//	// 陰影とテクスチャの色を合成
//	output.target0 = shadecolor * texcolor;
//	output.target1 = float4(1 - (shadecolor * texcolor).rgb, 1);
//
//	return output;
//}

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}