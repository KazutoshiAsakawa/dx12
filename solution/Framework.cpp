#include "Framework.h"
#include "FbxLoader.h"
#include "PostEffect.h"
#include "SpriteCommon.h"

void Framework::Run()
{
	// ゲームの初期化
	Initialize();

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
	Finalize();
}

void Framework::Initialize()
{
	winApp = new WinApp();
	winApp->Initialize();

	// DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	// FBXの初期化
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDev());

	// スプライト共通部分の初期化
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon->GetDev(), dxCommon->GetCmdList(), winApp->window_width, winApp->window_height);

	// デバッグテキスト
	debugText = DebugText::GetInstance();
	// デバッグテキスト用のテクスチャ番号を指定
	const int debugTextTexNumber = 0;
	// デバッグテキスト用のテクスチャ読み込み
	spriteCommon->LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText->Initialize(spriteCommon, debugTextTexNumber);

	//入力の初期化
	input = Input::GetInstance();
	input->Initialize(winApp);
	input->Update();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// 3Dオブジェクト静的初期化
	ObjObject3d::StaticInitialize(dxCommon->GetDev(), dxCommon->GetCmdList(), WinApp::window_width, WinApp::window_height);

	PostEffect::SetDevice(dxCommon->GetDev());
}

void Framework::Finalize()
{
	// FBXの解放
	FbxLoader::GetInstance()->Finalize();

	// デバッグテキスト解放
	debugText->Finalize();
	// オーディオ解放
	audio->Finalize();
	// DirectX解放
	delete dxCommon;
	// WindowsAPIの終了処理
	winApp->Finalize();
	//WindowsAPI解放
	delete winApp;
}

void Framework::Update()
{
	if (winApp->ProcessMessage())
	{
		//ゲームループを抜ける
		endRequest_ = true;
		return;
	}

	// 入力の更新
	input->Update();
	// シーンの更新
	SceneManager::GetInstance()->Update(dxCommon);

	if (input->TriggerKey(DIK_R)) {
		if (shaderNum == 0u) {
			shaderNum = 1u;
		}
		else {
			shaderNum = 0u;
		}
	}
}

void Framework::Draw()
{
//0
	PostEffect::GetInstance()->PreDrawScene(dxCommon->GetCmdList());

	// シーン描画
	SceneManager::GetInstance()->Draw(dxCommon);
	//0
	PostEffect::GetInstance()->PostDrawScene(dxCommon->GetCmdList());

	//1
	//PostEffect::GetInstance()->PreDrawScene(dxCommon->GetCmdList());

	//0
	// 背景スプライト、3Dオブジェクトの描画
	//PostEffect::GetInstance()->Draw(dxCommon->GetCmdList(), shaderNum);
	//1
	// PostEffect::GetInstance()->PostDrawScene(dxCommon->GetCmdList());


	// 描画前処理
	dxCommon->PreDraw();

	//1
	PostEffect::GetInstance()->Draw(dxCommon->GetCmdList(), shaderNum);

	// 前景スプライト描画準備
	SpriteCommon::GetInstance()->PreDraw();
	// 前景スプライトの描画
	SceneManager::GetInstance()->DrawFrontSprite(dxCommon);

	// デバッグテキスト描画
	debugText->DrawAll();

	dxCommon->PostDraw();
}