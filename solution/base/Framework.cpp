#include "Framework.h"
#include "FbxLoader.h"
#include "PostEffect.h"
#include "SpriteCommon.h"
#include "ParticleManager.h"
#include "ParticleLoad.h"
#include "LightGroup.h"

void Framework::Run() {
	// ゲームの初期化
	//Initialize();

	while (true)  // ゲームループ
	{
		// 毎フレーム更新
		Update();

		// 終了リクエストが来たら抜ける
		if (IsEndRequest()) {
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	// Finalize();
}

void Framework::Initialize() {
	winApp = WinApp::GetInstance();
	dxCommon = DirectXCommon::GetInstance();

	// FBXの初期化
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDev());

	// スプライト共通部分の初期化
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon->GetDev(), dxCommon->GetCmdList(), winApp->window_width, winApp->window_height);

	// デバッグテキスト
	debugText = DebugText::GetInstance();
	// デバッグテキスト初期化
	debugText->Initialize(spriteCommon, spriteCommon->LoadTexture(L"Resources/debugfont.png"));

	//入力の初期化
	input = Input::GetInstance();
	input->Initialize(winApp);
	input->Update();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	LightGroup::StaticInitialize(dxCommon->GetDev());
	ParticleLoad::GetInstance()->Initialize(dxCommon->GetDev());

	// 3Dオブジェクト静的初期化
	ObjObject3d::StaticInitialize(dxCommon->GetDev(), dxCommon->GetCmdList(), WinApp::window_width, WinApp::window_height);

	PostEffect::SetDevice(dxCommon->GetDev());
}

void Framework::Finalize() {
	// デバッグテキスト解放
	debugText->Finalize();
	// オーディオ解放
	audio->Finalize();
}

void Framework::Update() {
	if (winApp->ProcessMessage()) {
		//ゲームループを抜ける
		endRequest = true;
		return;
	}

	// 入力の更新
	input->Update();
	// シーンの更新
	SceneManager::GetInstance()->Update(dxCommon);
}

void Framework::Draw() {
	PostEffect::GetInstance()->PreDrawScene(dxCommon->GetCmdList());

	// シーン描画
	SceneManager::GetInstance()->Draw(dxCommon);

	PostEffect::GetInstance()->PostDrawScene(dxCommon->GetCmdList());

	// 描画前処理
	dxCommon->PreDraw();

	PostEffect::GetInstance()->Draw(dxCommon->GetCmdList(), shaderNum);

	// 前景スプライト描画準備
	SpriteCommon::GetInstance()->PreDraw();
	// 前景スプライトの描画
	SceneManager::GetInstance()->DrawFrontSprite(dxCommon);

	// デバッグテキスト描画
	debugText->DrawAll();

	dxCommon->PostDraw();
}