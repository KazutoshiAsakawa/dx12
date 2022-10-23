#include "Sprite.h"

#include <d3dx12.h>

using namespace DirectX;

Sprite* Sprite::Create(UINT texNumber, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	// �������m��
	Sprite* instance = new Sprite();
	// �C���X�^���X������
	instance->Initialize(texNumber, anchorpoint, isFlipX, isFlipY);

	return instance;
}

void Sprite::Initialize(UINT texNumber, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	HRESULT result = S_FALSE;

	// �����o�ϐ��ɏ�������
	this->texNumber = texNumber;
	this->anchorpoint = anchorpoint;
	this->isFlipX = isFlipX;
	this->isFlipY = isFlipY;

	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	// ���_�f�[�^
	VertexPosUv vertices[4];

	// �w��ԍ��̉摜���ǂݍ��ݍς݂Ȃ�
	if (spriteCommon->GetTexBuff(texNumber)) {
		// �e�N�X�`�����擾
		D3D12_RESOURCE_DESC resDesc = spriteCommon->GetTexBuff(texNumber)->GetDesc();

		// �X�v���C�g�̑傫�����摜�̉𑜓x�ɍ��킹��
		size = { (float)resDesc.Width, (float)resDesc.Height };
		texSize = { (float)resDesc.Width, (float)resDesc.Height };
	}

	// ���_�o�b�t�@����
	result = spriteCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff));

	// ���_�o�b�t�@�f�[�^�]��
	TransferVertexBuffer();

	// ���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	// �萔�o�b�t�@�̐���
	result = spriteCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));

	// �萔�o�b�t�@�Ƀf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1); // �F�w��iRGBA�j
	constMap->mat = spriteCommon->GetMatProjection();
	constBuff->Unmap(0, nullptr);
}

void Sprite::TransferVertexBuffer()
{
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	HRESULT result = S_FALSE;

	// ���_�f�[�^
	VertexPosUv vertices[] = {
		//     u     v
		{{}, {0.0f, 1.0f}}, // ����
		{{}, {0.0f, 0.0f}}, // ����
		{{}, {1.0f, 1.0f}}, // �E��
		{{}, {1.0f, 0.0f}}, // �E��
	};

	// �����A����A�E���A�E��
	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorpoint.x) * size.x;
	float right = (1.0f - anchorpoint.x) * size.x;
	float top = (0.0f - anchorpoint.y) * size.y;
	float bottom = (1.0f - anchorpoint.y) * size.y;

	if (isFlipX)
	{// ���E����ւ�
		left = -left;
		right = -right;
	}

	if (isFlipY)
	{// ���E����ւ�
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left, bottom,  0.0f }; // ����
	vertices[LT].pos = { left, top,     0.0f }; // ����
	vertices[RB].pos = { right, bottom, 0.0f }; // �E��
	vertices[RT].pos = { right, top,    0.0f }; // �E��

	// �w��ԍ��̉摜���ǂݍ��ݍς݂Ȃ�
	if (spriteCommon->GetTexBuff(texNumber)) {
		// �e�N�X�`�����擾
		D3D12_RESOURCE_DESC resDesc = spriteCommon->GetTexBuff(texNumber)->GetDesc();

		float tex_left = texLeftTop.x / resDesc.Width;
		float tex_right = (texLeftTop.x + texSize.x) / resDesc.Width;
		float tex_top = texLeftTop.y / resDesc.Height;
		float tex_bottom = (texLeftTop.y + texSize.y) / resDesc.Height;

		vertices[LB].uv = { tex_left,   tex_bottom }; // ����
		vertices[LT].uv = { tex_left,   tex_top }; // ����
		vertices[RB].uv = { tex_right,  tex_bottom }; // �E��
		vertices[RT].uv = { tex_right,  tex_top }; // �E��
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);
}

void Sprite::Update()
{
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();
	// ���[���h�s��̍X�V
	matWorld = XMMatrixIdentity();
	// Z����]
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));
	// ���s�ړ�
	matWorld *= XMMatrixTranslation(position.x, position.y, position.z);

	// �萔�o�b�t�@�̓]��
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matWorld * spriteCommon->GetMatProjection();
	constMap->color = color;
	constBuff->Unmap(0, nullptr);
}

void Sprite::Draw()
{
	if (isInvisible) {
		return;
	}

	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	ID3D12GraphicsCommandList* CommandList = spriteCommon->GetCommandList();

	// ���_�o�b�t�@���Z�b�g
	CommandList->IASetVertexBuffers(0, 1, &vbView);

	// ���[�g�p�����[�^0�Ԃɒ萔�o�b�t�@���Z�b�g
	CommandList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// ���[�g�p�����[�^1�ԂɃV�F�[�_���\�[�X�r���[���Z�b�g
	spriteCommon->SetGraphicsRootDescriptorTable(1, texNumber);

	// �|���S���̕`��i4���_�Ŏl�p�`�j
	CommandList->DrawInstanced(4, 1, 0, 0);
}
