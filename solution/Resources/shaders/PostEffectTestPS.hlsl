#include "PostEffectTest.hlsli"

Texture2D<float4> tex0 : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1);  // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float2 uv = floor(input.uv * mosaicNum) / mosaicNum;

	float4 colortex0;
	colortex0.r = tex0.Sample(smp, uv + float2(0.000f, 0)).r;
	colortex0.g = tex0.Sample(smp, uv + float2(0.000f, 0)).g;
	colortex0.b = tex0.Sample(smp, uv + float2(0.000f, 0)).b;

	float4 colortex1 = tex1.Sample(smp, uv);

	float4 color = colortex0;
	if (fmod(uv.y, 0.1f) < 0.05f) {
		//color = colortex1;
	}

	return float4(color.rgb, 1);
}