#include "TitleScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "DirectXCommon.h"
#include "Collision.h"
#include "ParticleLoad.h"

#include "PostEffect.h"
#include "Game.h"

using namespace DirectX;

void TitleScene::Initialize(DirectXCommon* dxcommon) {

	// Inputクラスのインスタンス(へのポインタ)を変数に入れておく
	input = Input::GetInstance();

	// スプライトの生成
	title["kitsunebi"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/kitsunebi.png"), { 0.0f, 0.0f }, false, false));
	title["pressS"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/pressS.png"), { 0.0f, 0.0f }, false, false));
	title["credit"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/credit.png"), { 0.0f, 0.0f }, false, false));

	title["pressS"]->SetIsInvisible(true);

	// カメラの初期化
	camera.reset(new DebugCamera(WinApp::window_width, WinApp::window_height));
	camera->SetEye({ 0, 5, -20 });
	camera->SetTarget({ 0, 0, 50 });

	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetPointLightActive(0, false);
	lightGroup->SetCircleShadowActive(0, true);

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

#pragma region プレイヤー
	// プレイヤー初期化
	player = std::make_unique<Player>();

#pragma endregion プレイヤー

#pragma region カメラ

	// カメラをプレイヤーの位置にセット
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.x += 10;
	eye.y += 5;
	eye.z += 30;
	camera->SetEye(eye);

#pragma endregion カメラ

	// パーティクル初期化
	ParticleLoad::GetInstance()->SetCamera(camera.get());

	// 更新処理の関数を入れる
	updateProcess = std::bind(&TitleScene::start, this);
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {
	if (input->PushKey(DIK_ESCAPE)) {
		Game::GetInstance()->SetEndRequest(true);
	}

	// シーン遷移
	updateProcess();

	{
		XMFLOAT3 eye = camera->GetEye();

		float length = 50;
		rad += 0.005f;
		eye.x = player->GetPosition().x + cos(rad) * length;
		eye.z = player->GetPosition().z + sin(rad) * length;

		camera->SetEye(eye);
	}

	/*lightGroup->SetSpotLightDir(0, XMLoadFloat3(&spotLightDir));
	lightGroup->SetSpotLightPos(0, spotLightPos);
	lightGroup->SetSpotLightColor(0, spotLightColor);
	lightGroup->SetSpotLightAtten(0, spotLightAtten);
	lightGroup->SetSpotLightFactorAngle(0, spotLightFactorAngle);*/
	lightGroup->SetCircleShadowDir(0, XMLoadFloat3(&circleShadowDir));
	lightGroup->SetCircleShadowCasterPos(0, player->GetPosition());
	lightGroup->SetCircleShadowAtten(0, circleShadowAtten);
	lightGroup->SetCircleShadowFactorAngle(0, circleShadowFactorAngle);

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
		auto color = title["pressS"]->GetColor();

		float rate = sinf((float)frame / 20.f);
		color.w = (rate + 2.f) / 5.f;

		title["pressS"]->SetColor(color);
	}

	// スプライト更新
	for (auto& i : title) {
		i.second->Update();
	}

	// フレームを進める
	frame++;
}

// シーン遷移
void TitleScene::start() {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 60;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcessにbossEntry関数をセット
		updateProcess = std::bind(&TitleScene::play, this);
		mosaicFrame = 0;
		// PressSpaceを表示する
		title["pressS"]->SetIsInvisible(false);
	} else {
		// モザイクがだんだん薄くなる演出
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void TitleScene::play() {

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
		ParticleLoad::GetInstance()->SetRenderAdd(2, 200,
			{ (float)rand() / RAND_MAX * 80.f - 40.f + shrinePos.x,
			50,
			(float)rand() / RAND_MAX * 80.f - 40.f },
			{ 0.f,-0.3f,0.f }, { 0.f,0.f,-0.00001f },
			1.5f, 1.5f, 0.0f, 720.0f, { 1.f, 0.f, 0.f }, { 1.f,0.f,0.f });
	}

	// スカイドームの位置
	skyDomeObj->SetPosition(player->GetPosition());

	if (input->TriggerKey(DIK_SPACE)) {
		updateProcess = std::bind(&TitleScene::end, this, "GAMEPLAY");
		title["pressS"]->SetIsInvisible(true);
	}
}

void TitleScene::end(const std::string& nextScene) {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 60;

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

void TitleScene::Draw(DirectXCommon* dxcommon) {
	// 3Dオブジェクト描画前処理
	ObjObject3d::PreDraw();

	// 地面
	groundObj->Draw();

	//スカイドームの描画
	skyDomeObj->Draw();

	// プレイヤーの描画
	player->Draw();

	// 神社
	shrineObj->Draw();
	// 篝火
	bonfireR->Draw();
	bonfireL->Draw();

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
	for (auto& i : title) {
		i.second->Draw();
	}
}

void TitleScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();
}