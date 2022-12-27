#include "TitleScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

void TitleScene::Initialize(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/title/back.png");
	SpriteCommon::GetInstance()->LoadTexture(2, L"Resources/title/kitsunebi.png");
	SpriteCommon::GetInstance()->LoadTexture(3, L"Resources/title/pressS.png");

	// �X�v���C�g�̐���
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

	// �V�[���؂�ւ�
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
