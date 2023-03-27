#include "ClearScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Collision.h"
#include "ParticleLoad.h"

#include "PostEffect.h"
#include "Game.h"
#include <random>

namespace {
	// 乱数
	float MyRand(float center, float range) {
		// 乱数生成器
		static std::mt19937_64 mt64(0);
		// 乱数生成器
		std::normal_distribution<float> rnd(center, range);
		// 乱数を生成
		return rnd(mt64);
	}
	float MyRandMinMax(float min, float max) {
		// 乱数生成器
		static std::mt19937_64 mt64(0);
		std::uniform_real_distribution<float> rnd(min, max);
		return rnd(mt64);
	}
	int MyRandMinMax(int min, int max) {
		// 乱数生成器
		static std::mt19937_64 mt64(0);
		std::uniform_int_distribution<int> rnd(min, max);
		return rnd(mt64);
	}
}

using namespace DirectX;

void ClearScene::Initialize(DirectXCommon* dxcommon) {

	// Inputクラスのインスタンス(へのポインタ)を変数に入れておく
	input = Input::GetInstance();

	// スプライトの生成
	gameOver["clear"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/gameclear/clear.png"), { 0.0f, 0.0f }, false, false));
	gameOver["pressS"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/gameclear/pressS.png"), { 0.0f, 0.0f }, false, false));

	gameOver["pressS"]->SetIsInvisible(true);

	// カメラの初期化
	camera.reset(new DebugCamera(WinApp::window_width, WinApp::window_height));
	camera->SetEye({ 0, 5, -20 });
	camera->SetTarget({ 0, 0, 50 });

	ObjObject3d::SetCamera(camera.get());

	// OBJからモデルデータを読み込む

	// スカイドーム
	skyDomeModel.reset(ObjModel::LoadFromObj("skydome"));
	skyDomeObj = ObjObject3d::Create();
	skyDomeObj->SetModel(skyDomeModel.get());
	skyDomeObj->SetScale({ 5,5,5 });

	// 地面
	groundModel.reset(ObjModel::LoadFromObj("ground"));
	groundModel->SetTiling({ 5.f,5.f });
	groundObj = ObjObject3d::Create();
	groundObj->SetModel(groundModel.get());
	groundObj->SetScale({ 100,100,100 });
	groundObj->SetPosition({ 0,-5,0 });

	// 神社
	shrineObj = ObjObject3d::Create();
	shrineObj->SetModel(ObjModel::LoadFromObj("shrine"));
	shrineObj->SetScale({ 3,3,3 });
	shrineObj->SetRotation({ 0,0,0 });
	shrineObj->SetPosition({ 0,-9,-11 });

	// 篝火
	bonfireR = ObjObject3d::Create();
	bonfireR->SetModel(ObjModel::LoadFromObj("bonfire"));
	bonfireR->SetScale({ 2,2,2 });
	bonfireR->SetPosition({ 5,-5,10 });

	bonfireL = ObjObject3d::Create();
	bonfireL->SetModel(ObjModel::LoadFromObj("bonfire"));
	bonfireL->SetScale({ 2,2,2 });
	bonfireL->SetPosition({ -5,-5,10 });

	//デバイスをセット
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//カメラをセット
	FbxObject3d::SetCamera(camera.get());
	//グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();

#pragma region プレイヤー
	// プレイヤー初期化
	player = ObjObject3d::Create();
	player->SetModel(ObjModel::LoadFromObj("fox"));
	player->SetRotation({ 0,0,0 });

#pragma endregion プレイヤー

#pragma region カメラ

	// カメラをプレイヤーの位置にセット
	camera->SetTarget(shrineObj->GetPosition());
	XMFLOAT3 eye = shrineObj->GetPosition();
	eye.x += 0;
	eye.y += 17;
	eye.z += 45;
	camera->SetEye(eye);

#pragma endregion カメラ

	// パーティクル初期化
	ParticleLoad::GetInstance()->SetCamera(camera.get());

	// 更新処理の関数を入れる
	updateProcess = std::bind(&ClearScene::start, this);

}

void ClearScene::Finalize() {
}

void ClearScene::Update() {
	if (input->PushKey(DIK_ESCAPE)) {
		Game::GetInstance()->SetEndRequest(true);
	}

	// シーン遷移
	updateProcess();

	// 篝火の炎パーティクル
	{
		constexpr int texNum = 1;
		constexpr XMFLOAT3 accel = { 0.f,0.f,0.f };
		constexpr float startScale = { 1.f };
		constexpr XMFLOAT3 startCol = { 0.7f,0.7f,0.3f };
		constexpr XMFLOAT3 endCol = { 1.f,0.f,0.f };
		XMFLOAT3 bonfireRPos = bonfireR->GetWorldPos();
		bonfireRPos.y += 4.f;
		bonfireRPos.x += MyRandMinMax(0.f, 0.125f);
		bonfireRPos.z += MyRandMinMax(0.f, 0.125f);
		XMFLOAT3 velocityR = { MyRand(0.f, 0.02f), 0.2f, MyRand(0.f, 0.02f) };
		ParticleLoad::GetInstance()->SetRenderAdd(texNum, MyRandMinMax(8, 24), bonfireRPos, velocityR, accel,
			startScale, 0.f, startCol, endCol);
		XMFLOAT3 bonfireLPos = bonfireL->GetWorldPos();
		bonfireLPos.y += 4.f;
		bonfireRPos.x += MyRandMinMax(0.f, 0.125f);
		bonfireRPos.z += MyRandMinMax(0.f, 0.125f);
		XMFLOAT3 velocityL = { MyRand(0.f, 0.02f), 0.2f, MyRand(0.f, 0.02f) };
		ParticleLoad::GetInstance()->SetRenderAdd(texNum, MyRandMinMax(8, 24), bonfireLPos, velocityL, accel,
			startScale, 0.f, startCol, endCol);
	}

	// 紅葉パーティクル
	{
		XMFLOAT3 shrinePos = shrineObj->GetWorldPos();
		ParticleLoad::GetInstance()->SetRenderAdd(2, 200, { (float)rand() / RAND_MAX * 80.f - 40.f + shrinePos.x,50,(float)rand() / RAND_MAX * 80.f - 40.f }, { 0.f,-0.3f,0.f }, { 0.f,0.f,-0.00001f },
			1.5f, 1.5f, 0.0f, 720.0f, { 0.7f, 0.7f, 0.3f }, { 1.f,0.f,0.f });
	}

	// キャラクターを神社の周りを走らせる
	/*{
		XMFLOAT3 playerPos = player->GetPosition();
		float length = 15;
		rad += 0.01f;
		playerPos.x = shrineObj->GetPosition().x + cos(rad) * length;
		playerPos.z = shrineObj->GetPosition().z + sin(rad) * length;

		playerPos.y = 0.1f;

		XMFLOAT3 playerRot = player->GetRotation();
		playerRot.y -= shrineObj->GetPosition().x + cos(rad) * length;
		player->SetRotation(playerRot);
		player->SetPosition(playerPos);
	}*/

	// パーティクル更新
	ParticleLoad::GetInstance()->Update();

	camera->Update();
	player->Update();
	groundObj->Update();
	skyDomeObj->Update();
	shrineObj->Update();
	bonfireR->Update();
	bonfireL->Update();

	{
		auto color = gameOver["pressS"]->GetColor();

		float rate = sinf((float)frame / 20.f);
		color.w = (rate + 2.f) / 5.f;

		gameOver["pressS"]->SetColor(color);
	}

	// スプライト更新
	for (auto& i : gameOver) {
		i.second->Update();
	}

	// フレームを進める
	frame++;
}

// シーン遷移
void ClearScene::start() {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 120;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcessにbossEntry関数をセット
		updateProcess = std::bind(&ClearScene::play, this);
		mosaicFrame = 0;
		gameOver["pressS"]->SetIsInvisible(false);
	} else {
		// モザイクがだんだん薄くなる演出
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void ClearScene::play() {

	// スカイドームの位置
	skyDomeObj->SetPosition(player->GetPosition());

	if (input->TriggerKey(DIK_SPACE)) {
		updateProcess = std::bind(&ClearScene::end, this, "TITLE");
		gameOver["pressS"]->SetIsInvisible(true);
	}
}

void ClearScene::end(const std::string& nextScene) {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 50;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		// 指定のエンド画面へ
		SceneManager::GetInstance()->ChangeScene(nextScene);
	} else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		rate = 1 - rate;// 1から0
		rate *= rate;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void ClearScene::Draw(DirectXCommon* dxcommon) {
	// 3Dオブジェクト描画前処理
	ObjObject3d::PreDraw();

	// 地面
	groundObj->Draw();

	//スカイドームの描画
	skyDomeObj->Draw();

	// 神社
	shrineObj->Draw();
	// 篝火
	bonfireR->Draw();
	bonfireL->Draw();

	// プレイヤーの描画
	player->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	ObjObject3d::PostDraw();
	// パーティクル描画
	ParticleLoad::GetInstance()->Draw(dxcommon->GetCmdList());

	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();

	// スプライト描画
	for (auto& i : gameOver) {
		i.second->Draw();
	}
}

void ClearScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();
}