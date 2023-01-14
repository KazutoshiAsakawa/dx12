#include "ClearScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "PostEffect.h"

using namespace DirectX;

void ClearScene::Initialize(DirectXCommon* dxcommon) {
	// スプライト共通テクスチャ読み込み
	int clearGrNum = SpriteCommon::GetInstance()->LoadTexture(L"Resources/clear.png");

	PostEffect::GetInstance()->SetMosaicNum(XMFLOAT2(WinApp::window_width, WinApp::window_height));

	// スプライトの生成
	sprite = Sprite::Create(clearGrNum, { 0,0 }, false, false);
}

void ClearScene::Finalize() {
	// スプライト個別解放
	delete sprite;
}

void ClearScene::Update() {
	Input* input = Input::GetInstance();

	// シーン切り替え
	if (input->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	// スプライト更新
	sprite->Update();
}

void ClearScene::Draw(DirectXCommon* dxcommon) {
	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
	// スプライト描画
	/*for (auto& sprite : sprites) {
		sprite->Draw();
	}*/
	sprite->Draw();
}
