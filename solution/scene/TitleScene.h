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
#include "LightGroup.h"

#include <Vector>
#include <memory>
#include <functional>
#include <unordered_map>

/// <summary>
/// ゲームプレイシーン
/// </summary>
class TitleScene : public BaseScene {
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
	std::unique_ptr<Player> player;

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
	std::unordered_map<std::string, std::unique_ptr<Sprite>> title;

	bool eyeControl = false;

	float rad = 0;

	LightGroup* lightGroup = nullptr;
	float ambientColor0[3] = { 1,1,1 };
	// 光線方向初期値
	float lightDir0[3] = { 0,0,1 };
	float lightColor0[3] = { 1,0,0 };

	float lightDir1[3] = { 0,1,0 };
	float lightColor1[3] = { 0,1,0 };

	float lightDir2[3] = { 1,0,0 };
	float lightColor2[3] = { 0,0,1 };

	float pointLightPos[3] = { 0,0,0 };
	float pointLightColor[3] = { 1,1,1 };
	float pointLightAtten[3] = { 0.3f,0.1f,0.1f };

	float spotLightDir[3] = { 0,-1,0 };
	float spotLightPos[3] = { 0,5,0 };
	float spotLightColor[3] = { 1,1,1 };
	float spotLightAtten[3] = { 0.0f,0.0f,0.0f };
	float spotLightFactorAngle[2] = { 20.0f, 30.0f };
};