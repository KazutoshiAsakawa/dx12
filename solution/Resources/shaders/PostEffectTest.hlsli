cbuffer cbuff0 : register(b0)
{
	float2 mosaicNum;
	// RGBずらし
	float2 shiftR;
	float2 shiftG;
	float2 shiftB;
	// 画面サイズ
	float2 windowSize;
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float2 uv  :TEXCOORD; // uv値
};
