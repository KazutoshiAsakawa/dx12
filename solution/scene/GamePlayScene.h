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

	/// <summary>
	/// 最初のシーン遷移
	/// </summary>
	void start();

	/// <summary>
	/// プレイ画面
	/// </summary>
	void play();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// 前景スプライト描画
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;

	/// <summary>
	/// スプライン曲線
	/// </summary>
	/// <param name="posints">座標</param>
	/// <param name="startIndex">始点</param>
	/// <param name="t">時間</param>
	/// <returns></returns>
	DirectX::XMVECTOR SplinePosition(const std::vector<DirectX::XMVECTOR>& posints, size_t startIndex, float t);

	// 敵を発生させる
	std::unique_ptr<Enemy>& EnemyAdd(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel);

	/// <summary>
	/// ダメージエフェクト
	/// </summary>
	/// <param name="maxFrame">最大フレーム</param>
	/// <param name="nowFrame">現在のフレーム</param>
	void DamageEffect(UINT maxFrame, UINT nowFrame);

private:
	// 背景スプライト
	std::unique_ptr<Sprite> sprite;
	// 照準スプライト
	std::unique_ptr<Sprite> aim;

	// ポーズ画面の枚数
	UINT pouseMax = 3;
	// ポーズ画面の配列
	std::vector<std::unique_ptr<Sprite>> pouseSprite;
	int pouse = 0;

	// 自機HP最大値
	UINT playerHpMax = 4;
	// HP画像の配列
	std::unique_ptr<Sprite> playerHpSprite;
	std::unique_ptr<Sprite> playerHpSlide;

	// 敵が当たって無い時のターゲット
	std::unique_ptr<GameObject> nullTarget;

	// スカイドーム
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// 地面
	std::unique_ptr<ObjModel> groundModel;
	std::unique_ptr<ObjObject3d> groundObj;

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

	// レーンの真ん中
	std::unique_ptr<GameObject> lane;

	// プレイヤー
	std::unique_ptr<Player> player;
	// 回避のクールタイム
	UINT avoidFrame = 0;
	UINT avoidFrameMax = 60;

	// 敵
	std::list<std::unique_ptr<Enemy>> enemy;

	// シーン遷移
	std::function<void()> updateProcess;

	// モザイク
	bool mosaicFlag = false;
	UINT mosaicFrame = 0;

	// RGBずらし
	UINT shiftNowFrame = 0;
	UINT shiftMaxFrame = 15;
	bool shiftFlag = false;

	// スプライン曲線の点
	std::vector<DirectX::XMVECTOR> points;
	size_t splineStartIndex;

	std::vector<std::vector<std::unique_ptr<ObjObject3d>>> wallObj;
	std::unique_ptr<ObjModel> wallModel;

	UINT frame = 0;

	UINT splineFrame = 0;

	// 敵を発生
	// 始まり,終わり
	std::list<std::pair<UINT, UINT>> enemyFrame;

	std::vector< std::vector<std::string>> csv;

	std::vector< std::vector<std::string>> splineCsv;

	// 敵の座標
	std::vector<DirectX::XMFLOAT3> enemyPos;

	// 増やした敵の数
	UINT addedEnemyNum = 0;

	// ポーズ画面管理
	bool pause = false;
};