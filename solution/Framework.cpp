#include "Framework.h"
#include "FbxLoader.h"
#include "PostEffect.h"
#include "SpriteCommon.h"
#include "ParticleManager.h"

void Framework::Run()
{
	// �Q�[���̏�����
	Initialize();

	while (true)  // �Q�[�����[�v
	{
		// ���t���[���X�V
		Update();

		// �I�����N�G�X�g�������甲����
		if (IsEndRequest()) {
			break;
		}
		// �`��
		Draw();
	}
	// �Q�[���̏I��
	Finalize();
}

void Framework::Initialize()
{
	winApp = WinApp::GetInstance();
	dxCommon = DirectXCommon::GetInstance();

	// FBX�̏�����
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDev());

	// �X�v���C�g���ʕ����̏�����
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon->GetDev(), dxCommon->GetCmdList(), winApp->window_width, winApp->window_height);

	// �f�o�b�O�e�L�X�g
	debugText = DebugText::GetInstance();
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const int debugTextTexNumber = 0;
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	spriteCommon->LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// �f�o�b�O�e�L�X�g������
	debugText->Initialize(spriteCommon, debugTextTexNumber);

	//���͂̏�����
	input = Input::GetInstance();
	input->Initialize(winApp);
	input->Update();

	// �I�[�f�B�I�̏�����
	audio = Audio::GetInstance();
	audio->Initialize();

	ParticleManager::GetInstance()->Initialize(dxCommon->GetDev());

	// 3D�I�u�W�F�N�g�ÓI������
	ObjObject3d::StaticInitialize(dxCommon->GetDev(), dxCommon->GetCmdList(), WinApp::window_width, WinApp::window_height);

	PostEffect::SetDevice(dxCommon->GetDev());
}

void Framework::Finalize()
{
	// FBX�̉��
	//FbxLoader::GetInstance()->Finalize();

	// �f�o�b�O�e�L�X�g���
	debugText->Finalize();
	// �I�[�f�B�I���
	audio->Finalize();
}

void Framework::Update()
{
	if (winApp->ProcessMessage())
	{
		//�Q�[�����[�v�𔲂���
		endRequest_ = true;
		return;
	}

	// ���͂̍X�V
	input->Update();
	// �V�[���̍X�V
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
	PostEffect::GetInstance()->PreDrawScene(dxCommon->GetCmdList());

	// �V�[���`��
	SceneManager::GetInstance()->Draw(dxCommon);

	PostEffect::GetInstance()->PostDrawScene(dxCommon->GetCmdList());

	// �`��O����
	dxCommon->PreDraw();

	PostEffect::GetInstance()->Draw(dxCommon->GetCmdList(), shaderNum);

	// �O�i�X�v���C�g�`�揀��
	SpriteCommon::GetInstance()->PreDraw();
	// �O�i�X�v���C�g�̕`��
	SceneManager::GetInstance()->DrawFrontSprite(dxCommon);

	// �f�o�b�O�e�L�X�g�`��
	debugText->DrawAll();

	dxCommon->PostDraw();
}