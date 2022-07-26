#include "PostEffectTest.hlsli"

Texture2D<float4> tex0 : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1 : register(t1);  // 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[




float4 main(VSOutput input) : SV_TARGET
{
	if (fmod(input.uv.y, 0.1f) < 0.05f) {
		return  tex0.Sample(smp, input.uv);
	}
	return tex1.Sample(smp, input.uv);
}

// return float4(1.0f - color.rgb,1);