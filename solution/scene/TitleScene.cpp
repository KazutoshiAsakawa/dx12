#include "TitleScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

void TitleScene::Initialize(DirectXCommon* dxcommon)
{
	// スプライト共通テクスチャ読み込み
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/title/back.png");
	SpriteCommon::GetInstance()->LoadTexture(2, L"Resources/title/kitsunebi.png");
	SpriteCommon::GetInstance()->LoadTexture(3, L"Resources/title/pressS.png");

	// スプライトの生成
	title["back"].reset(Sprite::Create(1, { 0.f, 0.f }));
	title["kitsunebi"].reset(Sprite::Create(2, { 0.0f, 0.0f }, false, false));
	title["pressS"].reset(Sprite::Create(3, { 0.0f, 0.0f }, false, false));
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	Input* input = Input::GetInstance();

	// シーン切り替え
	if (input->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	{
		auto color = title["pressS"]->GetColor();

		float rate = sinf((float)frame / 60.f);
		color.w = (rate + 2.f) / 3.f;

		title["pressS"]->SetColor(color);
	}

	// スプライト更新
	for (auto& i : title) {
		i.second->Update();
	}

	// フレームを進める
	frame++;
}

void TitleScene::Draw(DirectXCommon* dxcommon)
{
}

void TitleScene::DrawFrontSprite(DirectXCommon* dxcommon)
{
	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
	// スプライト描画
	for (auto& i : title) {
		i.second->Draw();
	}
}
