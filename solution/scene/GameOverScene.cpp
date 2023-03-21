#include "GameOverScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Collision.h"
#include "ParticleLoad.h"

#include "PostEffect.h"

using namespace DirectX;

void GameOverScene::Initialize(DirectXCommon* dxcommon) {

	// Input�N���X�̃C���X�^���X(�ւ̃|�C���^)��ϐ��ɓ���Ă���
	input = Input::GetInstance();

	// �X�v���C�g�̐���
	gameOver["die"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/gameover/die.png"), { 0.0f, 0.0f }, false, false));
	gameOver["pressS"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/gameover/pressS.png"), { 0.0f, 0.0f }, false, false));

	// �J�����̏�����
	camera.reset(new DebugCamera(WinApp::window_width, WinApp::window_height));
	camera->SetEye({ 0, 5, -20 });
	camera->SetTarget({ 0, 0, 50 });

	ObjObject3d::SetCamera(camera.get());

	// OBJ���烂�f���f�[�^��ǂݍ���

	// �X�J�C�h�[��
	skyDomeModel.reset(ObjModel::LoadFromObj("skydome"));
	skyDomeObj = ObjObject3d::Create();
	skyDomeObj->SetModel(skyDomeModel.get());
	skyDomeObj->SetScale({ 5,5,5 });

	// �n��
	groundModel.reset(ObjModel::LoadFromObj("ground"));
	groundModel->SetTiling({ 5.f,5.f });
	groundObj = ObjObject3d::Create();
	groundObj->SetModel(groundModel.get());
	groundObj->SetScale({ 100,100,100 });
	groundObj->SetPosition({ 0,-5,0 });

	// �_��
	shrineObj = ObjObject3d::Create();
	shrineObj->SetModel(ObjModel::LoadFromObj("shrine"));
	shrineObj->SetScale({ 3,3,3 });
	shrineObj->SetRotation({ 0,0,0 });
	shrineObj->SetPosition({ 0,-9,-11 });

	// ⾉�
	bonfireR = ObjObject3d::Create();
	bonfireR->SetModel(ObjModel::LoadFromObj("bonfire"));
	bonfireR->SetScale({ 2,2,2 });
	bonfireR->SetPosition({ 5,-5,10 });

	bonfireL = ObjObject3d::Create();
	bonfireL->SetModel(ObjModel::LoadFromObj("bonfire"));
	bonfireL->SetScale({ 2,2,2 });
	bonfireL->SetPosition({ -5,-5,10 });

	//�f�o�C�X���Z�b�g
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//�J�������Z�b�g
	FbxObject3d::SetCamera(camera.get());
	//�O���t�B�b�N�X�p�C�v���C������
	FbxObject3d::CreateGraphicsPipeline();

#pragma region �v���C���[
	// �v���C���[������
	// player = std::make_unique<Player>();
	player = ObjObject3d::Create();
	player->SetModel(ObjModel::LoadFromObj("fox"));
	player->SetRotation({ 0,0,90 });

#pragma endregion �v���C���[

#pragma region �J����

	// �J�������v���C���[�̈ʒu�ɃZ�b�g
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.x += 0;
	eye.y += 2;
	eye.z += 25;
	camera->SetEye(eye);

#pragma endregion �J����

	// �p�[�e�B�N��������
	ParticleLoad::GetInstance()->SetCamera(camera.get());

	// �X�V�����̊֐�������
	updateProcess = std::bind(&GameOverScene::start, this);

}

void GameOverScene::Finalize() {

}

void GameOverScene::Update() {


	// �V�[���J��
	updateProcess();

	// ⾉΂̉��p�[�e�B�N��
	{
		constexpr XMFLOAT3 velocity = { 0.f,0.2f,0.f };
		constexpr XMFLOAT3 accel = { 0.f,0.f,0.f };
		constexpr float startScale = { 1.f };
		constexpr XMFLOAT3 startCol = { 0.7f,0.7f,0.3f };
		constexpr XMFLOAT3 endCol = { 1.f,0.f,0.f };


		XMFLOAT3 bonfireRPos = bonfireR->GetPosition();
		bonfireRPos.y += 4.f;
		ParticleLoad::GetInstance()->SetRenderAdd(1, rand() % 20, bonfireRPos, velocity, accel,
			startScale, (float)rand() / RAND_MAX * 0.5f, startCol, endCol);

		XMFLOAT3 bonfireLPos = bonfireL->GetPosition();
		bonfireLPos.y += 4.f;
		ParticleLoad::GetInstance()->SetRenderAdd(1, rand() % 20, bonfireLPos, velocity, accel,
			startScale, (float)rand() / RAND_MAX * 0.5f, startCol, endCol);

	}

	/*{
		XMFLOAT3 eye = camera->GetEye();

		float length = 50;
		rad += 0.005f;
		eye.x = player->GetPosition().x + cos(rad) * length;
		eye.z = player->GetPosition().z + sin(rad) * length;

		camera->SetEye(eye);
	}*/

	// �p�[�e�B�N���X�V
	ParticleLoad::GetInstance()->Update();

	camera->Update();

	player->Update();



	groundObj->Update();
	skyDomeObj->Update();
	shrineObj->Update();
	bonfireR->Update();
	bonfireL->Update();

	{
		auto color = gameOver["pressS"]->GetColor();

		float rate = sinf((float)frame / 20.f);
		color.w = (rate + 2.f) / 5.f;

		gameOver["pressS"]->SetColor(color);
	}

	// �X�v���C�g�X�V
	for (auto& i : gameOver) {
		i.second->Update();
	}

	// �t���[����i�߂�
	frame++;
}

// �V�[���J��
void GameOverScene::start() {
	// ���U�C�N�������鎞��
	constexpr UINT mosaicFrameMax = 120;

	// ���U�C�N�̎��Ԃ��ő�܂ł�������play�֐��ɕς���
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcess��bossEntry�֐����Z�b�g
		updateProcess = std::bind(&GameOverScene::play, this);
		mosaicFrame = 0;
	} else {
		// ���U�C�N�����񂾂񔖂��Ȃ鉉�o
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void GameOverScene::play() {

	// �X�J�C�h�[���̈ʒu
	skyDomeObj->SetPosition(player->GetPosition());

	if (input->TriggerKey(DIK_SPACE)) {
		updateProcess = std::bind(&GameOverScene::end, this, "TITLE");
	}
}

void GameOverScene::end(const std::string& nextScene) {
	// ���U�C�N�������鎞��
	constexpr UINT mosaicFrameMax = 50;

	// ���U�C�N�̎��Ԃ��ő�܂ł�������play�֐��ɕς���
	if (++mosaicFrame > mosaicFrameMax) {
		// �w��̃G���h��ʂ�
		SceneManager::GetInstance()->ChangeScene(nextScene);
	} else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		rate = 1 - rate;// 1����0
		rate *= rate;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void GameOverScene::Draw(DirectXCommon* dxcommon) {
	// 3D�I�u�W�F�N�g�`��O����
	ObjObject3d::PreDraw();

	// �n��
	groundObj->Draw();

	//�X�J�C�h�[���̕`��
	skyDomeObj->Draw();

	// �_��
	shrineObj->Draw();
	// ⾉�
	bonfireR->Draw();
	bonfireL->Draw();

	// �v���C���[�̕`��
	player->Draw();

	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	ObjObject3d::PostDraw();
	// �p�[�e�B�N���`��
	ParticleLoad::GetInstance()->Draw(dxcommon->GetCmdList());

	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();

	// �X�v���C�g�`��
	for (auto& i : gameOver) {
		i.second->Draw();
	}
}

void GameOverScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();
}