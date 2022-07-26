#pragma once
#include "Sprite.h"
//#include "DirectXCommon.h"
class PostEffect
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	PostEffect();
	PostEffect(const PostEffect& postEffect) = delete;
	void operator=(const PostEffect& postEffect) = delete;

public:
	static PostEffect* GetInstance();

	/// <summary>
	/// ���_�f�[�^�\����
	/// </summary>
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos; // xyz���W
		DirectX::XMFLOAT2 uv;  // uv���W
	};
	/// <summary>
	/// �萔�o�b�t�@�p�f�[�^�\����
	/// </summary>
	struct ConstBufferData
	{
		DirectX::XMFLOAT2 mosaicNum{};
	};

	static const float clearColor[4];

	/// <summary>
	/// ������
	/// <summary>
	void Initialize();

	//void Draw(DirectXCommon* dxCom);
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT shaderNum);


	/// <summary>
	/// �V�[���`��O����
	/// <summary>
	/// <param name = "cmdlist">�R�}���h���X�g</param>
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �V�[���`��㏈��
	/// <summary>
	/// <param name = "cmdlist">�R�}���h���X�g</param>
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �p�C�v���C������
	/// </summary>
	void CreateGraphicsPipelineState(UINT shaderNum = 0, const wchar_t* PSpath = L"Resources/shaders/PostEffectTestPS.hlsl");

	inline void SetMosaicNum(DirectX::XMFLOAT2 mosaicnum) { this->mosaicNum = mosaicnum, constBuffDirty = true; }

	static void SetDevice(ID3D12Device* device);

private: // �����o�ϐ�
	static ID3D12Device* device_;

	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff[2];
	// SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	// �[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	// RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	// DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;

	// �O���t�B�b�N�X�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState[2];
	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;

	bool constBuffDirty = true;
	// ���U�C�N�ׂ̍���
	DirectX::XMFLOAT2 mosaicNum{};
};

