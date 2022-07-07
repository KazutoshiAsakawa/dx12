#include "TitleScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

void TitleScene::Initialize(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/title.png");

	// �X�v���C�g�̐���
	sprite = Sprite::Create(1, { 0,0 }, false, false);
}

void TitleScene::Finalize()
{
	// �X�v���C�g�ʉ��
	delete sprite;
}

void TitleScene::Update()
{
	Input* input = Input::GetInstance();

	// ���W����
	if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_RIGHT) || input->TriggerKey(DIK_LEFT))
	{

	}
	// �V�[���؂�ւ�
	if (input->TriggerKey(DIK_RETURN))
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	// �X�v���C�g�X�V
	sprite->Update();
}

void TitleScene::Draw(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();
	// �X�v���C�g�`��
	/*for (auto& sprite : sprites) {
		sprite->Draw();
	}*/
	sprite->Draw();
}
