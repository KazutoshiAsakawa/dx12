#include "GameOverScene.h"
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

using namespace DirectX;

void GameOverScene::Initialize(DirectXCommon* dxcommon) {

	// Inputクラスのインスタンス(へのポインタ)を変数に入れておく
	input = Input::GetInstance();

	// スプライトの生成
	gameOver["die"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/gameover/die.png"), { 0.0f, 0.0f }, false, false));
	gameOver["pressS"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/gameover/pressS.png"), { 0.0f, 0.0f }, false, false));

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
	player->SetRotation({ 0,0,90 });

#pragma endregion プレイヤー

#pragma region カメラ

	// カメラをプレイヤーの位置にセット
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.x += 0;
	eye.y += 2;
	eye.z += 25;
	camera->SetEye(eye);

#pragma endregion カメラ

	// パーティクル初期化
	ParticleLoad::GetInstance()->SetCamera(camera.get());

	// 更新処理の関数を入れる
	updateProcess = std::bind(&GameOverScene::Start, this);

}

void GameOverScene::Finalize() {
}

void GameOverScene::Update() {
	if (input->PushKey(DIK_ESCAPE)) {
		Game::GetInstance()->SetEndRequest(true);
	}

	// シーン遷移
	updateProcess();

	WinApp* winApp = WinApp::GetInstance();

	// 篝火の炎パーティクル
	{
		constexpr int texNum = 1;
		constexpr XMFLOAT3 accel = { 0.f,0.f,0.f };
		constexpr float startScale = { 1.f };
		constexpr XMFLOAT3 startCol = { 0.7f,0.7f,0.3f };
		constexpr XMFLOAT3 endCol = { 1.f,0.f,0.f };
		XMFLOAT3 bonfireRPos = bonfireR->GetWorldPos();
		bonfireRPos.y += 4.f;
		bonfireRPos.x += winApp->MyRandMinMax(0.f, 0.125f);
		bonfireRPos.z += winApp->MyRandMinMax(0.f, 0.125f);
		XMFLOAT3 velocityR = { winApp->MyRand(0.f, 0.02f), 0.2f, winApp->MyRand(0.f, 0.02f) };
		ParticleLoad::GetInstance()->SetRenderAdd(texNum, winApp->MyRandMinMax(8, 24), bonfireRPos, velocityR, accel,
			startScale, 0.f, startCol, endCol);
		XMFLOAT3 bonfireLPos = bonfireL->GetWorldPos();
		bonfireLPos.y += 4.f;
		bonfireRPos.x += winApp->MyRandMinMax(0.f, 0.125f);
		bonfireRPos.z += winApp->MyRandMinMax(0.f, 0.125f);
		XMFLOAT3 velocityL = { winApp->MyRand(0.f, 0.02f), 0.2f, winApp->MyRand(0.f, 0.02f) };
		ParticleLoad::GetInstance()->SetRenderAdd(texNum, winApp->MyRandMinMax(8, 24), bonfireLPos, velocityL, accel,
			startScale, 0.f, startCol, endCol);
	}

	// 紅葉パーティクル
	{
		XMFLOAT3 shrinePos = shrineObj->GetWorldPos();
		ParticleLoad::GetInstance()->SetRenderAdd(2, 200, { (float)rand() / RAND_MAX * 80.f - 40.f + shrinePos.x,50,(float)rand() / RAND_MAX * 80.f - 40.f }, { 0.f,-0.3f,0.f }, { 0.f,0.f,-0.00001f },
			1.5f, 1.5f, 0.0f, 720.0f, { 1.f,0.f,0.f }, { 1.f,0.f,0.f });
	}

	// パーティクル更新
	ParticleLoad::GetInstance()->Update();

	camera->Update();
	player->Update();
	groundObj->Update();
	skyDomeObj->Update();
	shrineObj->Update();
	bonfireR->Update();
	bonfireL->Update();

	// 点滅
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
void GameOverScene::Start() {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 120;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcessにbossEntry関数をセット
		updateProcess = std::bind(&GameOverScene::Play, this);
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

void GameOverScene::Play() {

	// スカイドームの位置
	skyDomeObj->SetPosition(player->GetPosition());

	if (input->TriggerKey(DIK_SPACE)) {
		updateProcess = std::bind(&GameOverScene::End, this, "TITLE");
		gameOver["pressS"]->SetIsInvisible(true);
	}
}

void GameOverScene::End(const std::string& nextScene) {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 50;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		// 指定のエンド画面へ
		SceneManager::GetInstance()->ChangeScene(nextScene);
		ParticleLoad::GetInstance()->ClearParticle();
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

void GameOverScene::Draw(DirectXCommon* dxcommon) {
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

void GameOverScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();
}