#include "EndScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

void EndScene::Initialize(DirectXCommon* dxcommon)
{
	// スプライト共通テクスチャ読み込み
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/end.png");

	// スプライトの生成
	sprite = Sprite::Create(1, { 0,0 }, false, false);
}

void EndScene::Finalize()
{
	// スプライト個別解放
	delete sprite;
}

void EndScene::Update()
{
	Input* input = Input::GetInstance();

	// 座標操作
	if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_RIGHT) || input->TriggerKey(DIK_LEFT))
	{

	}
	// シーン切り替え
	if (input->TriggerKey(DIK_RETURN))
	{
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	// スプライト更新
	sprite->Update();
}

void EndScene::Draw(DirectXCommon* dxcommon)
{
	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
	// スプライト描画
	/*for (auto& sprite : sprites) {
		sprite->Draw();
	}*/
	sprite->Draw();
}
