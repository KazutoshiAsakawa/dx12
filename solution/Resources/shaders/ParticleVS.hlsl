#include "Particle.hlsli"

VSOutput main(float4 pos : POSITION, float scale : TEXCOORD, float4 color : COLOR)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = pos;
	output.scale = scale;
	output.color = color;
	return output;
}