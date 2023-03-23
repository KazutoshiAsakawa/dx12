#include "TitleScene.h"
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
#include "Game.h"
#include <random>

namespace {
	// ����
	float MyRand(float center, float range) {
		// ����������
		static std::mt19937_64 mt64(0);
		// ����������
		std::normal_distribution<float> rnd(center, range);
		// �����𐶐�
		return rnd(mt64);
	}
	float MyRandMinMax(float min, float max) {
		// ����������
		static std::mt19937_64 mt64(0);
		std::uniform_real_distribution<float> rnd(min, max);
		return rnd(mt64);
	}
	int MyRandMinMax(int min, int max) {
		// ����������
		static std::mt19937_64 mt64(0);
		std::uniform_int_distribution<int> rnd(min, max);
		return rnd(mt64);
	}
}

using namespace DirectX;

void TitleScene::Initialize(DirectXCommon* dxcommon) {

	// Input�N���X�̃C���X�^���X(�ւ̃|�C���^)��ϐ��ɓ���Ă���
	input = Input::GetInstance();

	// �X�v���C�g�̐���
	title["kitsunebi"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/kitsunebi.png"), { 0.0f, 0.0f }, false, false));
	title["pressS"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/pressS.png"), { 0.0f, 0.0f }, false, false));
	title["credit"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/title/credit.png"), { 0.0f, 0.0f }, false, false));

	title["pressS"]->SetIsInvisible(true);

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
	player = std::make_unique<Player>();

#pragma endregion �v���C���[

#pragma region �J����

	// �J�������v���C���[�̈ʒu�ɃZ�b�g
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.x += 10;
	eye.y += 5;
	eye.z += 30;
	camera->SetEye(eye);

#pragma endregion �J����

	// �p�[�e�B�N��������
	ParticleLoad::GetInstance()->SetCamera(camera.get());

	// �X�V�����̊֐�������
	updateProcess = std::bind(&TitleScene::start, this);
}

void TitleScene::Finalize() {

}

void TitleScene::Update() {
	if (input->PushKey(DIK_ESCAPE)) {
		Game::GetInstance()->SetEndRequest(true);
	}

	// �V�[���J��
	updateProcess();

	// ⾉΂̉��p�[�e�B�N��
	{
		constexpr int texNum = 1;
		constexpr XMFLOAT3 accel = { 0.f,0.f,0.f };
		constexpr float startScale = { 1.f };
		constexpr XMFLOAT3 startCol = { 0.7f,0.7f,0.3f };
		constexpr XMFLOAT3 endCol = { 1.f,0.f,0.f };
		XMFLOAT3 bonfireRPos = bonfireR->GetWorldPos();
		bonfireRPos.y += 4.f;
		bonfireRPos.x += MyRandMinMax(0.f, 0.125f);
		bonfireRPos.z += MyRandMinMax(0.f, 0.125f);
		XMFLOAT3 velocityR = { MyRand(0.f, 0.02f), 0.2f, MyRand(0.f, 0.02f) };
		ParticleLoad::GetInstance()->SetRenderAdd(texNum, MyRandMinMax(8, 24), bonfireRPos, velocityR, accel,
			startScale, 0.f, startCol, endCol);
		XMFLOAT3 bonfireLPos = bonfireL->GetWorldPos();
		bonfireLPos.y += 4.f;
		bonfireRPos.x += MyRandMinMax(0.f, 0.125f);
		bonfireRPos.z += MyRandMinMax(0.f, 0.125f);
		XMFLOAT3 velocityL = { MyRand(0.f, 0.02f), 0.2f, MyRand(0.f, 0.02f) };
		ParticleLoad::GetInstance()->SetRenderAdd(texNum, MyRandMinMax(8, 24), bonfireLPos, velocityL, accel,
			startScale, 0.f, startCol, endCol);
	}

	// �g�t�p�[�e�B�N��
	{
		XMFLOAT3 shrinePos = shrineObj->GetWorldPos();
		ParticleLoad::GetInstance()->SetRenderAdd(2, 200, { (float)rand() / RAND_MAX * 80.f - 40.f + shrinePos.x,50,(float)rand() / RAND_MAX * 80.f - 40.f }, { 0.f,-0.3f,0.f }, { 0.f,0.f,-0.00001f },
			1.5f, 1.5f, 0.0f,720.0f, { 0.7f, 0.7f, 0.3f }, { 1.f,0.f,0.f });
	}

	{
		XMFLOAT3 eye = camera->GetEye();
		
		float length = 50;
		rad += 0.005f;
		eye.x = player->GetPosition().x + cos(rad) * length;
		eye.z = player->GetPosition().z + sin(rad) * length;

		camera->SetEye(eye);
	}

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
		auto color = title["pressS"]->GetColor();

		float rate = sinf((float)frame / 20.f);
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

// �V�[���J��
void TitleScene::start() {
	// ���U�C�N�������鎞��
	constexpr UINT mosaicFrameMax = 120;

	// ���U�C�N�̎��Ԃ��ő�܂ł�������play�֐��ɕς���
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcess��bossEntry�֐����Z�b�g
		updateProcess = std::bind(&TitleScene::play, this);
		mosaicFrame = 0;
		// PressSpace��\������
		title["pressS"]->SetIsInvisible(false);
	} else {
		// ���U�C�N�����񂾂񔖂��Ȃ鉉�o
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void TitleScene::play() {

	// �X�J�C�h�[���̈ʒu
	skyDomeObj->SetPosition(player->GetPosition());

	if (input->TriggerKey(DIK_SPACE)) {
		updateProcess = std::bind(&TitleScene::end, this, "GAMEPLAY");
		title["pressS"]->SetIsInvisible(true);
	}
}

void TitleScene::end(const std::string& nextScene) {
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

void TitleScene::Draw(DirectXCommon* dxcommon) {
	// 3D�I�u�W�F�N�g�`��O����
	ObjObject3d::PreDraw();

	// �n��
	groundObj->Draw();

	//�X�J�C�h�[���̕`��
	skyDomeObj->Draw();

	// �v���C���[�̕`��
	player->Draw();

	// �_��
	shrineObj->Draw();
	// ⾉�
	bonfireR->Draw();
	bonfireL->Draw();

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
	for (auto& i : title) {
		i.second->Draw();
	}
}

void TitleScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();
}