#include "GameOverScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "PostEffect.h"

using namespace DirectX;

void GameOverScene::Initialize(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	int dieGrNum = SpriteCommon::GetInstance()->LoadTexture(L"Resources/die.png");

	PostEffect::GetInstance()->SetMosaicNum(XMFLOAT2(WinApp::window_width, WinApp::window_height));

	// �X�v���C�g�̐���
	sprite = Sprite::Create(dieGrNum, { 0,0 }, false, false);
}

void GameOverScene::Finalize()
{
	// �X�v���C�g�ʉ��
	delete sprite;
}

void GameOverScene::Update()
{
	Input* input = Input::GetInstance();

	// �V�[���؂�ւ�
	if (input->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	// �X�v���C�g�X�V
	sprite->Update();
}

void GameOverScene::Draw(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();
	// �X�v���C�g�`��
	/*for (auto& sprite : sprites) {
		sprite->Draw();
	}*/
	sprite->Draw();
}
