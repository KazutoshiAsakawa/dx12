#include "GameOverScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "PostEffect.h"

using namespace DirectX;

void GameOverScene::Initialize(DirectXCommon* dxcommon)
{
	// スプライト共通テクスチャ読み込み
	int dieGrNum = SpriteCommon::GetInstance()->LoadTexture(L"Resources/die.png");

	PostEffect::GetInstance()->SetMosaicNum(XMFLOAT2(WinApp::window_width, WinApp::window_height));

	// スプライトの生成
	sprite = Sprite::Create(dieGrNum, { 0,0 }, false, false);
}

void GameOverScene::Finalize()
{
	// スプライト個別解放
	delete sprite;
}

void GameOverScene::Update()
{
	Input* input = Input::GetInstance();

	// シーン切り替え
	if (input->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	// スプライト更新
	sprite->Update();
}

void GameOverScene::Draw(DirectXCommon* dxcommon)
{
	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
	// スプライト描画
	/*for (auto& sprite : sprites) {
		sprite->Draw();
	}*/
	sprite->Draw();
}
