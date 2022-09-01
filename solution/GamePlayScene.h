#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "ObjObject3d.h"
#include "FbxObject3d.h"

#include "DebugCamera.h"
#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "TrackingCamera.h"

#include <Vector>
#include <memory>
#include <functional>

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

	void start();
	void play();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// 前景スプライト描画
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;


	DirectX::XMVECTOR splinePosition(const std::vector<DirectX::XMVECTOR>& posints, size_t startIndex, float t);

	// 敵を発生させる
	std::unique_ptr<Enemy>& enemyAdd(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel);

	

private:
	// 背景
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Sprite> aim;

	// スカイドーム
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// 弾
	std::unique_ptr<ObjModel> pBulletModel = nullptr;
	// 弾の大きさ
	float pBulletScale = 2;

	// 敵
	std::unique_ptr<ObjModel> enemyModel = nullptr;
	// 敵の大きさ
	float enemyScale = 2;
	// カメラ
	std::unique_ptr<TrackingCamera> camera;

	// プレイヤー
	std::unique_ptr<Player> player;
	std::list<std::unique_ptr<Enemy>> enemy;

	std::function<void()> updateProcess;

	// モザイク
	bool mosaicFlag = false;
	UINT mosaicFrame = 0;

	std::vector<DirectX::XMVECTOR> points;
	size_t splineStartIndex;

	UINT frame = 0;

	// 敵を発生
	// 始まり,終わり
	std::list<std::pair<UINT, UINT>> enemyFrame;


	std::vector< std::vector<std::string>> csv;

	std::vector<DirectX::XMFLOAT3> enemyPos;

	UINT addedEnemyNum = 0;
};