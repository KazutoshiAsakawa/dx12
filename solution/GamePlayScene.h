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
/// ゲームプレイシーン
/// </summary>
class GamePlayScene : public BaseScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxcommon) override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// 前景スプライト描画
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

