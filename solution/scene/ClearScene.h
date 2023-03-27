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
#include "Boss.h"

#include <Vector>
#include <memory>
#include <functional>
#include <unordered_map>

/// <summary>
/// ゲームプレイシーン
/// </summary>
class ClearScene : public BaseScene {
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
	/// 開始
	/// </summary>
	void start();

	/// <summary>
	/// ゲーム本編
	/// </summary>
	void play();

	/// <summary>
	///	終わったときの演出
	/// </summary>
	/// <param name="nextScene">次のシーン</param>
	void end(const std::string& nextScene);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// 前景スプライト描画
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;

private:
	Input* input = nullptr;

	// スカイドーム
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// 地面
	std::unique_ptr<ObjModel> groundModel;
	std::unique_ptr<ObjObject3d> groundObj;

	// カメラ
	std::unique_ptr<DebugCamera> camera;

	// プレイヤー
	// std::unique_ptr<Player> player;
	std::unique_ptr<ObjObject3d> player;

	std::function<void()> updateProcess;

	// 神社
	std::unique_ptr<ObjObject3d> shrineObj;

	// 篝火
	std::unique_ptr<ObjObject3d> bonfireR;
	std::unique_ptr<ObjObject3d> bonfireL;

	// モザイク
	bool mosaicFlag = false;
	UINT mosaicFrame = 0;

	std::vector<DirectX::XMVECTOR> points;
	size_t splineStartIndex;

	// フレーム
	UINT frame;

	// タイトル画面
	std::unordered_map<std::string, std::unique_ptr<Sprite>> gameOver;

	bool eyeControl = false;

	float rad = 0;
};