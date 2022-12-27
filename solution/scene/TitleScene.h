#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "ObjObject3d.h"
#include "DirectXCommon.h"

#include <unordered_map>

/// <summary>
/// �^�C�g���V�[��
/// </summary>
class TitleScene : public BaseScene
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(DirectXCommon* dxcommon) override;

	/// <summary>
	/// �I������
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;
	void DrawFrontSprite(DirectXCommon* dxcommon) override;

private:
	// �t���[��
	UINT frame;

	// �^�C�g�����
	std::unordered_map<std::string, std::unique_ptr<Sprite>> title;

};

