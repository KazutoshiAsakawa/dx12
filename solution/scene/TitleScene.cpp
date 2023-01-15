#include "TitleScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

void TitleScene::Initialize(DirectXCommon* dxcommon)
{
	// �X�v���C�g�̐���
	title["back"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/back.png"), { 0.f, 0.f }));
	title["kitsunebi"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/kitsunebi.png"), { 0.0f, 0.0f }, false, false));
	title["pressS"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/pressS.png"), { 0.0f, 0.0f }, false, false));
	title["credit"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/credit.png"), { 0.0f, 0.0f }, false, false));
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	Input* input = Input::GetInstance();

	// �V�[���؂�ւ�
	if (input->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	{
		auto color = title["pressS"]->GetColor();

		float rate = sinf((float)frame / 60.f);
		color.w = (rate + 2.f) / 5.f;

		title["pressS"]->SetColor(color);
	}

	// �X�v���C�g�X�V
	for (auto& i : title) {
		i.second->Update();
	}

	// �t���[����i�߂�
	frame++;
}

void TitleScene::Draw(DirectXCommon* dxcommon)
{
}

void TitleScene::DrawFrontSprite(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();
	// �X�v���C�g�`��
	for (auto& i : title) {
		i.second->Draw();
	}
}
