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
class BossScene : public BaseScene
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

	void start();	// 開始
	void bossEntry();	// ボス登場
	void play();	// ゲーム本編
	void killEffect();	// ボスを倒したエフェクト
	void end(const std::string& nextScene);	// 終了

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// 前景スプライト描画
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;


	DirectX::XMVECTOR SplinePosition(const std::vector<DirectX::XMVECTOR>& posints, size_t startIndex, float t);

	void DamageEffect(UINT maxFrame, UINT nowFrame);

private:
	// マウス
	POINT mousePosDiff{};

	std::unique_ptr<Sprite> aim;

	// 操作画像
	std::unordered_map<std::string, std::unique_ptr<Sprite>> operationSprite;

	// スカイドーム
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// 地面
	std::unique_ptr<ObjModel> groundModel;
	std::unique_ptr<ObjObject3d> groundObj;

	// ポーズ画面の枚数
	UINT pouseMax = 3;
	// ポーズ画面の配列
	std::vector<std::unique_ptr<Sprite>> pouseSprite;
	int pouse = 0;

	// 自機HP最大値
	UINT playerHpMax = 30;
	// HP画像
	std::unique_ptr<Sprite> playerHpSprite;
	std::unique_ptr<Sprite> playerHpSlide;

	// 弾
	std::unique_ptr<ObjModel> pBulletModel = nullptr;
	// 弾の大きさ
	float pBulletScale = 1;

	// 敵
	std::unique_ptr<ObjModel> enemyModel = nullptr;
	// 敵の大きさ
	float enemyScale = 2;
	// カメラ
	std::unique_ptr<TrackingCamera> camera;
	float cameraLengthDef;

	// プレイヤー
	std::unique_ptr<Player> player;
	// 回避のクールタイム
	UINT avoidFrame = 0;
	UINT avoidFrameMax = 60;

	// ボス
	std::unique_ptr<Boss> boss;
	// ボスHP最大値
	UINT bossHpMax = 30;
	float bossScale;
	// ボスHP画像
	std::unique_ptr<Sprite> bossHpSprite;
	std::unique_ptr<Sprite> bossHpSlide;

	bool killBossFlag = false;

	std::function<void()> updateProcess;

	// モザイク
	bool mosaicFlag = false;
	UINT mosaicFrame = 0;

	// RGBずらし
	UINT nowFrame = 0;
	UINT maxFrame = 15;
	bool shiftFlag = false;

	std::vector<DirectX::XMVECTOR> points;
	size_t splineStartIndex;

	UINT frame = 0;

	UINT nowEffectFrame = 0;

	UINT bossEntryNowFrame = 0;
};