#pragma once

#include"SpriteCommon.h"

#include <wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>

/// <summary>
/// �X�v���C�g1������\���N���X
/// </summary>
class Sprite
{
public:
	// ���_�f�[�^
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos; // xyz���W
		DirectX::XMFLOAT2 uv;  // uv���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		DirectX::XMFLOAT4 color; // �F (RGBA)
		DirectX::XMMATRIX mat;   // �R�c�ϊ��s��
	};
	/// <summary>
	/// �X�v���C�g����
	/// </summary>
	/// <param name="texNumber">�e�N�X�`���ԍ�</param>
	/// <param name="anchorpoint">�A���J�[�|�C���g</param>
	/// <param name="isFlipX">X���]���邩</param>
	/// <param name="isFlipY">Y���]���邩</param>
	/// <returns>�X�v���C�g</returns>
	static Sprite* Create(UINT texNumber, DirectX::XMFLOAT2 anchorpoint = { 0.5f,0.5f }, bool isFlipX = false, bool isFlipY = false);

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(UINT texNumber, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY);

	/// <summary>
	/// ���_�o�b�t�@�̓]��
	/// </summary>
	void TransferVertexBuffer();

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetSize(const DirectX::XMFLOAT2& size) { this->size = size; }
	void SetTexLeftTop(const DirectX::XMFLOAT2& texLeftTop){ this->texLeftTop = texLeftTop;}
	void SetTexSize(const DirectX::XMFLOAT2& texSize){ this->texSize = texSize;}
	void SetColor(const DirectX::XMFLOAT4& color){ this->color = color;}

	inline const DirectX::XMFLOAT2& GetSize() {return size; };

private:
	//���_�o�b�t�@;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�萔�o�b�t�@;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	// Z�����̉�]�p
	float rotation = 0.0f;
	// ���W
	DirectX::XMFLOAT3 position = { 0,0,0 };
	// ���[���h�s��
	DirectX::XMMATRIX matWorld;
	// �F(RGBA)
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };// 1, 1, 1, 1
	// �e�N�X�`���ԍ�
	UINT texNumber = 0;
	// �傫��
	DirectX::XMFLOAT2 size = { 100, 100 };
	// �A���J�[�|�C���g
	DirectX::XMFLOAT2 anchorpoint = { 0.5f, 0.5f };
	// ���E���]
	bool isFlipX = false;
	// �㉺���]
	bool isFlipY = false;
	// �e�N�X�`��������W
	DirectX::XMFLOAT2 texLeftTop = { 0, 0 };
	// �e�N�X�`���؂�o���T�C�Y
	DirectX::XMFLOAT2 texSize = { 100, 100 };
	// ��\��
	bool isInvisible = false;
};

