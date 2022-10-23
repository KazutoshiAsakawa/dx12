#pragma once

#include <wrl.h>
#include <d3d12.h>

/// <summary>
/// �p�C�v���C���̃Z�b�g
/// </summary>
struct PipelineSet
{
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate;
	// ���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;
};