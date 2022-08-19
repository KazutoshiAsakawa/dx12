#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "ObjObject3d.h"
#include "FbxObject3d.h"

#include "DebugCamera.h"
#include "Player.h"
#include "PlayerBullet.h"

#include <Vector>
#include <memory>

/// <summary>
/// �Q�[���v���C�V�[��
/// </summary>
class GamePlayScene : public BaseScene
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

	/// <summary>
	/// �O�i�X�v���C�g�`��
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;

private:
	// std::vector<Sprite*> sprites;
	Sprite* sprite = nullptr;
	ObjModel* model = nullptr;
	std::unique_ptr<ObjObject3d> object3d;
	// Object3d* object3d = nullptr;

	std::unique_ptr<ObjModel> pBulletModel = nullptr;

	std::unique_ptr<DebugCamera> camera;

	//FbxModel* fbxModel = nullptr;
	//FbxObject3d* fbxObj = nullptr;

	std::unique_ptr<Player> player;

	std::vector<PlayerBullet> playerBullet;

	bool mosaicFlag = false;
};

