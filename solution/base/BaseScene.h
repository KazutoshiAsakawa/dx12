#pragma once
#include "DirectXCommon.h"

// �O���錾
class SceneManager;

/// <summary>
/// �V�[�����
/// </summary>
class BaseScene
{
public:
	virtual ~BaseScene() = default;

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize(DirectXCommon* dxcommon) = 0;

	/// <summary>
	/// �I������
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw(DirectXCommon* dxcommon) = 0;

	/// <summary>
	/// �O�i�X�v���C�g�`��
	/// </summary>
	virtual void DrawFrontSprite(DirectXCommon* dxcommon){};
};

