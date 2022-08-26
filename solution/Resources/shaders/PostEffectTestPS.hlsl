#include "PostEffectTest.hlsli"

Texture2D<float4> tex0 : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1);  // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float vignette(float2 uv) {
	// 中心からの距離
	float2 dista2 = distance(uv, 0.5f);
	// 距離が遠いほど暗くする
	return sqrt(dista2.x * dista2.x + dista2.y * dista2.y) * 0.25;
}


float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
	// 描画座標と取得座標の距離
	float d = distance(drawUV, pickUV);
	return exp(-(d * d) / (2 * sigma * sigma));
}

float4 GaussianBlur(Texture2D<float4> tex, float2 uv, float sigma = 0.005, float stepWidth = 0.001)
{
	float _ShiftWidth = 0.005f;
	float _ShiftNum = 3;

	float totalWeight = 0;
	float4 col = float4(0, 0, 0, 1);

	for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
	{
		for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
		{
			float2 pickUV = uv + float2(px, py);
			float weight = Gaussian(uv, pickUV, sigma);
			col += tex0.Sample(smp, pickUV) * weight;
			totalWeight += weight;
		}
	}
	col.rgb = col.rgb / totalWeight;
	return col;
}

float4 RgbShift(Texture2D<float4> tex, float2 uv, float2 shiftR, float2 shiftG, float2 shiftB) {
	float4 retCol;
	retCol.r = tex.Sample(smp, uv + shiftR).r;
	retCol.g = tex.Sample(smp, uv + shiftG).g;
	retCol.b = tex.Sample(smp, uv + shiftB).b;
	retCol.a = 1.0f;

	return retCol;
}

float4 RgbShiftGaussianBlur(Texture2D<float4> tex, float2 uv, float2 shiftR, float2 shiftG, float2 shiftB) {
	float4 retCol;
	retCol.r = GaussianBlur(tex, uv + shiftR).r;
	retCol.g = GaussianBlur(tex, uv + shiftG).g;
	retCol.b = GaussianBlur(tex, uv + shiftB).b;
	retCol.a = 1.0f;

	return retCol;
}

float4 main(VSOutput input) : SV_TARGET
{
	// mosaic
	float2 mosaicUv = floor(input.uv * mosaicNum) / mosaicNum;

	// RgbShift
	float4 rgbShiftColor = RgbShift(tex0,
									mosaicUv,
									float2(0.000f, 0.0000f),
									float2(0.000f, 0.0000f),
									float2(0.000f, 0.0000f));

	float4 retColor = rgbShiftColor;
	float4 colortex1 = tex0.Sample(smp, mosaicUv);

	/*if (fmod(mosaicUv.y, 0.1f) < 0.05f) {
		retColor = colortex1;
		return float4(1.0f - retColor.rgb, 1);
	}*/

	// vignatte
	float vignVal = vignette(mosaicUv);

	// Gaussian
	// float4 blurColor = GaussianBlur(tex1, mosaicUv);

	return float4(retColor.rgb - vignVal, 1.0f);

}

// return float4(1.0f - color.rgb,1);