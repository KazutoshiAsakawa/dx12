#include "PostEffectTest.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float vignette(float2 uv, float intensity = 0.1f)
{
	// 中心からの距離
	float2 dista2 = distance(uv, 0.5f);
	// 距離が遠いほど暗くする
	return sqrt(dista2.x * dista2.x + dista2.y * dista2.y) * intensity;
}

// ガウス関数
// 近いほど大きい値を返す
// 遠いほど小さい値を返す
float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
	// 描画座標と取得座標の距離
	float d = distance(drawUV, pickUV);
	return exp(-(d * d) / (2 * sigma * sigma));
}

// ガウスぼかし
// 【ぼかす】周囲のピクセルの色を混ぜる。
// 【ガウスぼかし】近いピクセルほど多く混ぜる。混ぜる割合はガウス関数で決める
float4 GaussianBlur(Texture2D<float4> tex, float2 uv, float sigma = 0.005, float stepWidth = 0.001)
{
	float _ShiftWidth = 0.005f;
	float _ShiftNum = 3;

	// 合計でどのくらい混ぜたかを記録する変数
	float totalWeight = 0;

	// 最終的な色を入れる変数
	float4 col = float4(0, 0, 0, 1);

	for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
	{
		for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
		{
			// どこの色を混ぜるか
			float2 pickUV = uv + float2(px, py);

			// どのくらい混ぜるか(重み)
			// 近いピクセルほど大きく、
			// 遠いピクセルほど小さくなる
			float weight = Gaussian(uv, pickUV, sigma);

			// 混ぜる色
			float4 texCol = tex.Sample(smp, pickUV);

			// 混ぜる
			// 混ぜる色に重みを反映させたものを混ぜる
			col += texCol * weight;

			// どのくらい混ぜたかを記録
			totalWeight += weight;
		}
	}
	// 0~1にする
	col.rgb = col.rgb / totalWeight;
	return col;
}

// 高光度部分を抽出したものをぼかす
float4 GaussianBlurHighLumi(Texture2D<float4> tex, float2 uv, float sigma = 0.005, float stepWidth = 0.001, float threthreshold = 0.9f)
{
	float _ShiftWidth = 0.005f;
	float _ShiftNum = 3;

	// 合計でどのくらい混ぜたかを記録する変数
	float totalWeight = 0;

	// 最終的な色を入れる変数
	float4 col = float4(0, 0, 0, 1);

	for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
	{
		for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
		{
			// どこの色を混ぜるか
			float2 pickUV = uv + float2(px, py);

			// どのくらい混ぜるか(重み)
			// 近いピクセルほど大きく、
			// 遠いピクセルほど小さくなる
			float weight = Gaussian(uv, pickUV, sigma);

			// 混ぜる色
			// 高光度部分を抽出したものを混ぜる
			// 閾値以下なら0(混ぜない)
			// 閾値以上なら1(混ぜる)
			// なので、真っ白もしくは真っ黒のどちらか
			float4 texCol = step(threthreshold, tex.Sample(smp, pickUV));
			// ↓ガウスぼかし
			// float4 texCol = tex.Sample(smp, pickUV);

			// 混ぜる
			// 混ぜる色に重みを反映させたものを混ぜる
			col.rgb += texCol * weight;

			// どのくらい混ぜたかを記録
			totalWeight += weight;
		}
	}
	// 0~1にする
	col.rgb = col.rgb / totalWeight;
	return col;
}

float4 RgbShift(Texture2D<float4> tex, float2 uv, float2 shiftR, float2 shiftG, float2 shiftB)
{
	float4 retCol;
	retCol.r = tex.Sample(smp, uv + shiftR).r;
	retCol.g = tex.Sample(smp, uv + shiftG).g;
	retCol.b = tex.Sample(smp, uv + shiftB).b;
	retCol.a = 1.0f;

	return retCol;
}

// RGBずらしとガウスぼかしを両方やる
float4 RgbShiftGaussianBlur(Texture2D<float4> tex, float2 uv, float2 shiftR, float2 shiftG, float2 shiftB)
{
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
	float4 rgbShiftColor = RgbShift(tex0, mosaicUv, shiftR, shiftG, shiftB);

	float4 retColor = rgbShiftColor;
	float4 colortex1 = tex0.Sample(smp, mosaicUv);

	// vignatte
	float vignVal = vignette(mosaicUv, 0.4f);

	// 高光度部分をぼかしたもの
	float3 highLumi = GaussianBlurHighLumi(tex0, input.uv).rgb;

	// 高光度部分をぼかしたものを足してブルームにする
	retColor.rgb += highLumi.rgb;

	// ビネットを反映
	retColor.rgb -= vignVal;

	return float4(retColor.rgb, 1.f);

}