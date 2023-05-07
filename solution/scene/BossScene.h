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
class BossScene : public BaseScene {
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
	void Start();

	/// <summary>
	/// ボス登場
	/// </summary>
	void BossEntry();

	/// <summary>
	/// ゲーム本編
	/// </summary>
	void Play();

	/// <summary>
	/// ボスを倒したエフェクト
	/// </summary>
	void KillEffect();

	/// <summary>
	/// プレイヤーの死亡演出
	/// </summary>
	void DeathPlayer();

	/// <summary>
	///	終わったときの演出
	/// </summary>
	/// <param name="nextScene">次のシーン</param>
	void End(const std::string& nextScene);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// 前景スプライト描画
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;


	DirectX::XMVECTOR SplinePosition(const std::vector<DirectX::XMVECTOR>& posints, size_t startIndex, float t);

	/// <summary>
	/// ダメージエフェクト
	/// </summary>
	/// <param name="maxFrame">最大フレーム</param>
	/// <param name="nowFrame">現在のフレーム</param>
	void DamageEffect(UINT maxFrame, UINT nowFrame);

	/// <summary>
	/// プレイヤーの移動と回避
	/// </summary>
	void PlayerMove();

	/// <summary>
	/// 照準と敵のスクリーン座標の当たり判定
	/// </summary>
	void CollisionAimAndEnemyScreenPos();

	/// <summary>
	/// 敵と自機の弾の当たり判定
	/// </summary>
	void CollisionEnemyAndPlayerBullet();

	/// <summary>
	/// 自機と敵の弾の当たり判定
	/// </summary>
	void CollisionPlayerAndEnemyBullet();

private:
	Input* input = nullptr;

	// マウス
	POINT mousePosDiff{};

	// 照準スプライト
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
	UINT pauseMax = 3;
	// ポーズ画面の配列
	std::vector<std::unique_ptr<Sprite>> pauseSprite;
	int pause = 0;

	// 自機HP最大値
	UINT playerHpMax = 30;
	// HP画像
	DirectX::XMFLOAT2 playerHpSpriteSize;
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

	UINT shotInterval = 0;
	UINT shotIntervalMax = 15;

	// ボス
	std::unique_ptr<Boss> boss;
	// ボスHP最大値
	UINT bossHpMax = 30;
	float bossScale;
	// ボスHP画像
	DirectX::XMFLOAT2 bossHpSpriteSize;
	std::unique_ptr<Sprite> bossHpSprite;
	std::unique_ptr<Sprite> bossHpSlide;

	// ボスの名前画像
	std::unique_ptr<Sprite> bossText;


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

	std::string bgmFileName;
};