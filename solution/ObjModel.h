#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
#include <wrl.h>
#include <d3dx12.h>

/// <summary>
/// 3D���f��
/// </summary>
class ObjModel
{
public:	// �T�u�N���X
	// ���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos; // xyz���W
		DirectX::XMFLOAT3 normal; // �@���x�N�g��
		DirectX::XMFLOAT2 uv;  // uv���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		DirectX::XMFLOAT3 ambient; // �A���r�G���g�W��
		float pad1; // �p�f�B���O
		DirectX::XMFLOAT3 diffuse; // �f�B�t���[�Y�W��
		float pad2; // �p�f�B���O
		DirectX::XMFLOAT3 specular; // �X�y�L�����[�W��
		float alpha;	// �A���t�@
	};

	// �}�e���A��
	struct Material
	{
		std::string name;	// �}�e���A����
		DirectX::XMFLOAT3 ambient;	// �A���r�G���g�e���x
		DirectX::XMFLOAT3 diffuse;	// �f�B�t���[�Y�e���x
		DirectX::XMFLOAT3 specular;	// �X�y�L�����[�e���x
		float alpha;		// �A���t�@
		std::string textureFilename;	// �e�N�X�`���t�@�C����
		// �R���X�g���N�^
		Material() {
			ambient = { 0.3f, 0.3f, 0.3f };
			diffuse = { 0.0f, 0.0f, 0.0f };
			specular = { 0.0f, 0.0f, 0.0f };
			alpha = 1.0f;
		}
	};

public:	// �ÓI�����o�֐�
	/// <summary>
	/// OBJ�t�@�C������3D���f����ǂݍ���
	/// </summary>
	/// <param name="modelname">���f����</param>
	/// <returns>���f��</returns>
	static ObjModel* LoadFromObj(const std::string& modelname);

	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <returns>����</returns>
	bool LoadTexture(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	/// <param name="rootParamIndexMaterial">�}�e���A���̃��[�g�p�����[�^�ԍ��w��</param>
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);

	static void SetDevice(ID3D12Device* device) {ObjModel::device = device;}

private: // �����o�ϐ�
	// �f�o�C�X(�؂�Ă���)
	static ID3D12Device* device;

	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices_;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices_;
	// �}�e���A��
	Material material_;
	// �e�N�X�`���o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff_;
	// �f�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap_;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize_;
	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// �C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_;
	// �}�e���A���p�萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB1_;
private:
	void LoadFromOBJInternal(const std::string& modelname);

	// �f�X�N���v�^�q�[�v�̏�����
	void InitializeDescriptorHeap();

	// �e��o�b�t�@�̐���
	void CreateBuffers();
};

