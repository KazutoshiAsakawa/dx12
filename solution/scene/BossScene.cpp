#include "BossScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Collision.h"
#include "ParticleManager.h"

#include "PostEffect.h"

using namespace DirectX;

void BossScene::Initialize(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/gameplay.png");
	SpriteCommon::GetInstance()->LoadTexture(2, L"Resources/aim.png");

	// �X�v���C�g�̐���
	sprite.reset(Sprite::Create(1, { 0,0 }, false, false));
	aim.reset(Sprite::Create(2));
	aim->SetPosition({ WinApp::window_width / 2.0f ,WinApp::window_height / 2.0f,0.f });

	// �J�����̏�����
	camera.reset(new TrackingCamera());
	//camera->SetEye({ 0, 5, -20 });
	// camera->SetTarget({ 0, 0, 50 });

	ObjObject3d::SetCamera(camera.get());

	// OBJ���烂�f���f�[�^��ǂݍ���

	// �X�J�C�h�[��
	skyDomeModel.reset(ObjModel::LoadFromObj("skydome"));
	skyDomeObj = ObjObject3d::Create();
	skyDomeObj->SetModel(skyDomeModel.get());
	skyDomeObj->SetScale({ 5,5,5 });

	// �n��
	groundModel.reset(ObjModel::LoadFromObj("ground"));
	groundModel->SetTiling({ 10.f,10.f });
	groundObj = ObjObject3d::Create();
	groundObj->SetModel(groundModel.get());
	groundObj->SetScale({ 100,100,100 });
	groundObj->SetPosition({ 0,-5,0 });

	// ���@�̓ǂݍ���
	pBulletModel.reset(ObjModel::LoadFromObj("playerBullet"));
	// �G�̓ǂݍ���
	enemyModel.reset(ObjModel::LoadFromObj("obake"));

	//�f�o�C�X���Z�b�g
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//�J�������Z�b�g
	FbxObject3d::SetCamera(camera.get());
	//�O���t�B�b�N�X�p�C�v���C������
	FbxObject3d::CreateGraphicsPipeline();
	// �v���C���[������
	player = std::make_unique<Player>();

	// �J�������v���C���[�̈ʒu�ɃZ�b�g
	camera->SetTrackingTarget(player.get());
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.z -= 50;
	eye.y += 10;
	camera->SetEye(eye);

	// �{�X
	boss.reset(new Boss(enemyModel.get(), { 0.f,0.f,20.f }));

	boss->SetPhaseApproach();
	boss->SetAttackTarget(player.get());

	boss->SetBulletModel(enemyModel.get());

	// �p�[�e�B�N��������
	ParticleManager::GetInstance()->SetCamera(camera.get());

	// �X�V�����̊֐�������
	updateProcess = std::bind(&BossScene::start, this);

	// �����ǂݍ���
	Audio::GetInstance()->LoadWave("Alarm01.wav");
}

void BossScene::Finalize()
{
	DamageEffect(1, 1);
}

void BossScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		pause = !pause;
	}

	if (!pause) {

		// �}�E�X�̌Œ�
		{
			POINT oldMousePos = Input::GetInstance()->GetMousePos();

			Input::GetInstance()->SetMousePos({ long(WinApp::window_width / 2.f),long(WinApp::window_height / 2.f) });
			// �O���W�Ƃ̍���
			mousePosDiff = Input::GetInstance()->GetMousePos();
			mousePosDiff.x -= oldMousePos.x;
			mousePosDiff.y -= oldMousePos.y;
		}

		// �V�[���J��
		updateProcess();

		// �p�[�e�B�N���X�V
		ParticleManager::GetInstance()->Update();

		camera->Update();

		//fbxObj->Update();
		player->Update();
		//}

		boss->Update();

		groundObj->Update();
		skyDomeObj->Update();

		// �X�v���C�g�X�V
		sprite->Update();

		// aim->SetPosition({player->GetScreenAimPos().x,player->GetScreenAimPos().y,0});
		// aim->SetPosition({ (float)Input::GetInstance()->GetMousePos().x,(float)Input::GetInstance()->GetMousePos().y,0 });
		aim->Update();
	}
}

// �V�[���J��
void BossScene::start()
{
	Input* input = Input::GetInstance();
	// ���U�C�N�������鎞��
	constexpr UINT mosaicFrameMax = 40;

	// ���U�C�N�̎��Ԃ��ő�܂ł�������play�֐��ɕς���
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcess��play�֐����Z�b�g
		updateProcess = std::bind(&BossScene::play, this);
	}
	else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void BossScene::play()
{
	Input* input = Input::GetInstance();

	{
		char tmp[32]{};
		sprintf_s(tmp, 32, "%.2f,%.2f,%.2f", player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);
		DebugText::GetInstance()->Print(tmp, 0, 100);
	}

	// ���[���̈ʒu
	{
		skyDomeObj->SetPosition(player->GetPosition());
	}

	// �v���C���[�̈ړ��Ɖ��
	{
		const bool hitW = Input::GetInstance()->PushKey(DIK_W);
		const bool hitS = Input::GetInstance()->PushKey(DIK_S);
		const bool hitA = Input::GetInstance()->PushKey(DIK_A);
		const bool hitD = Input::GetInstance()->PushKey(DIK_D);
		const bool hitZ = Input::GetInstance()->PushKey(DIK_Z);
		const bool hitX = Input::GetInstance()->PushKey(DIK_X);

		const bool hitSpace = Input::GetInstance()->TriggerKey(DIK_SPACE);

		if (avoidFrame >= 1) {
			avoidFrame--;
		}

		if (hitW || hitS || hitA || hitD || hitZ || hitX) {
			auto pos = player->GetPosition();
			float moveSpeed = 0.2f;
			if (hitSpace && avoidFrame == 0) {
				moveSpeed *= 10;
				avoidFrame = avoidFrameMax;
			}

			// �O�����ƉE�����̒P�ʃx�N�g�������
			XMVECTOR forwardVec = XMVectorSet(0, 0, 1, 1);
			XMVECTOR rightVec = XMVectorSet(1, 0, 0, 1);
			// �v���C���[�̉�]�ɍ��킹�ĉ�]������
			forwardVec = XMVector3Rotate(forwardVec, XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(player->GetRotation().x),
				XMConvertToRadians(player->GetRotation().y),
				XMConvertToRadians(player->GetRotation().z)));

			rightVec = XMVector3Rotate(rightVec, XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(player->GetRotation().x),
				XMConvertToRadians(player->GetRotation().y),
				XMConvertToRadians(player->GetRotation().z)));
			// �傫����moveSpeed�ɂ���
			forwardVec = XMVectorScale(forwardVec, moveSpeed);
			rightVec = XMVectorScale(rightVec, moveSpeed);

			XMFLOAT3 forward;
			XMStoreFloat3(&forward, forwardVec);

			XMFLOAT3 right;
			XMStoreFloat3(&right, rightVec);

			if (hitW) {
				pos.x += forward.x;
				pos.y += forward.y;
				pos.z += forward.z;
			}
			else if (hitS) {
				pos.x -= forward.x;
				pos.y -= forward.y;
				pos.z -= forward.z;
			}

			if (hitD) {
				pos.x += right.x;
				pos.y += right.y;
				pos.z += right.z;
			}
			else if (hitA) {
				pos.x -= right.x;
				pos.y -= right.y;
				pos.z -= right.z;
			}

			player->SetPosition(pos);
		}
	}

	// �v���C���[�̉�]
	{
		XMFLOAT3 playerRot = player->GetRotation();
		constexpr float rotSpeed = 0.1f;

		playerRot.y -= rotSpeed * mousePosDiff.x;
		playerRot.x -= rotSpeed * mousePosDiff.y;
		player->SetRotation(playerRot);
	}

	if (boss->GetAlive())
	{
		// �摜�̍���ƉE��
		XMFLOAT2 aimLT = { aim->GetPosition().x - aim->GetSize().x / 2,
		 aim->GetPosition().y - aim->GetSize().y / 2 };

		XMFLOAT2 aimRB = { aim->GetPosition().x + aim->GetSize().x / 2,
		 aim->GetPosition().y + aim->GetSize().y / 2 };

		// �G�̏ꏊ
		XMFLOAT2 enemyPos;

		bool flag = false;

		// �Ə��ƓG�̃X�N���[�����W�̓����蔻��
		player->SetShotTarget(nullptr);

		enemyPos = { boss->GetFloat2ScreenPos().x,boss->GetFloat2ScreenPos().y };

		// �����蔻��
		if (aimLT.x <= enemyPos.x && aimLT.y <= enemyPos.y &&
			aimRB.x >= enemyPos.x && aimRB.y >= enemyPos.y) {
			flag = true;
			player->SetShotTarget(boss.get());
		}

		{
			char tmp[128];
			sprintf_s(tmp, 128, "boss : %.2f,%.2f", enemyPos.x, enemyPos.y);
			DebugText::GetInstance()->Print(tmp, 0, 0);
			sprintf_s(tmp, 128, "mouse : %.2f,%.2f", aim->GetPosition().x, aim->GetPosition().y);
			DebugText::GetInstance()->Print(tmp, 0, 20);
		}

		if (flag) {
			aim->SetColor({ 1,0,0,1 });
			// �Ə��������Ă����獶�N���b�N
			if (input->TriggerMouse(Input::LEFT)) {
				// ���@�̒e�̔���
				player->Shot(pBulletModel.get(), pBulletScale);
			}
		}
		else {
			aim->SetColor({ 1,1,1,1 });

			if (input->TriggerMouse(Input::LEFT)) {
				// ���@�̒e�̔���
				player->Shot(pBulletModel.get(), pBulletScale);
			}
		}
	}
	else {// �{�X�����񂾂�
		SceneManager::GetInstance()->ChangeScene("END");
	}

	// �G�Ǝ��@�̒e�̓����蔻��
	{
		Sphere pBulletShape;

		for (auto& pb : player->GetBullet()) {
			if (!pb.GetAlive())continue;
			pBulletShape.center = XMLoadFloat3(&pb.GetPosition());
			pBulletShape.radius = pb.GetScale().x;

			// �Փ˔��������
			Sphere enemyShape;
			enemyShape.center = XMLoadFloat3(&boss->GetPosition());
			enemyShape.radius = boss->GetScale().x;

			if (!boss->GetAlive()) break;

			// ���������������
			if (Collision::CheckSphere2Sphere(pBulletShape, enemyShape)) {
				boss->SetAlive(false);
				pb.SetAlive(false);
				aim->SetColor({ 1,1,1,1 });

				// �p�[�e�B�N���̔���
				ParticleManager::GetInstance()->CreateParticle(boss->GetPosition(), 100, 10, 10);
			}
		}
	}

	// �G�̒e�Ǝ��@
	{
		Sphere playerShape;

		playerShape.center = XMLoadFloat3(&player->GetPosition());
		playerShape.radius = player->GetScale().z;

		// �Փ˔��������
		if (player->GetAlive()) {
			for (auto& eb : boss->GetBullet()) {
				Sphere eBulletShape;
				eBulletShape.center = XMLoadFloat3(&eb.GetPosition());
				eBulletShape.radius = eb.GetScale().z;

				// ���������������
				if (Collision::CheckSphere2Sphere(playerShape, eBulletShape)) {
					eb.SetAlive(false);				// �G�̒e������
					player->Damage(1);				// �v���C���[�Ƀ_���[�W
					shiftFlag = true;				// RGB���炵������
					nowFrame = 0;
					if (player->GetHp() == 0) {		// �̗͂�0�ɂȂ�����
						player->SetAlive(false);

						SceneManager::GetInstance()->ChangeScene("END");
					}
				}
			}
		}

	}

	if (shiftFlag) {

		DamageEffect(maxFrame, nowFrame);
		nowFrame++;
		if (maxFrame < nowFrame) {
			shiftFlag = false;
			nowFrame = 0;
		}
	}

	// ���U�C�N�؂�ւ�
	{
		const bool TriggerP = input->TriggerKey(DIK_P);
		if (TriggerP) {
			// ���U�C�N�ׂ̍���
			// �E�C���h�E�T�C�Y�Ɠ����ׂ����̃��U�C�N = ���U�C�N�Ȃ�
			DirectX::XMFLOAT2 mosaicNum{ WinApp::window_width, WinApp::window_height };

			// ���U�C�N��������ꍇ�́A�ׂ�����ύX
			if (mosaicFlag) {
				mosaicNum = DirectX::XMFLOAT2(100, 100);
			}

			// ���U�C�N��������
			PostEffect::GetInstance()->SetMosaicNum(mosaicNum);

			// �t���O�𔽓]
			mosaicFlag = !mosaicFlag;
		}
	}

	frame++;
}

void BossScene::Draw(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();
	// �X�v���C�g�`��
	sprite->Draw();

	// 3D�I�u�W�F�N�g�`��O����
	ObjObject3d::PreDraw();

	// �n��
	groundObj->Draw();

	//�X�J�C�h�[���̕`��
	skyDomeObj->Draw();

	// �v���C���[�̕`��
	player->Draw();

	boss->Draw();

	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	ObjObject3d::PostDraw();
	// �p�[�e�B�N���`��
	ParticleManager::GetInstance()->Draw(dxcommon->GetCmdList());
	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();
}

void BossScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();
	aim->Draw();
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(1.f, 0.f, 1.f, 0.5f));

	if (pause) {
		ImGui::SetNextWindowPos(ImVec2(10.f, 10.f));
		ImGui::SetNextWindowSize(ImVec2(WinApp::window_width - 20, WinApp::window_height - 20));
		ImGui::Begin("pause", nullptr, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text(u8"ESC�Ŗ߂�");
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y + ImGui::GetWindowSize().y));
		ImGui::End();
	}
	else {
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		ImGui::Begin("aaa", nullptr, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text(u8"�t���[�� %u", frame);
		ImGui::Text(u8"�̗� %u", player->GetHp());
		ImGui::Text(u8"WASD:�ړ�");
		ImGui::Text(u8"���N���b�N:����");
		ImGui::Text(u8"�}�E�X���� %d,%d", mousePosDiff.x, mousePosDiff.y);
		ImGui::Text(u8"�}�E�X %d,%d", Input::GetInstance()->GetMousePos().x, Input::GetInstance()->GetMousePos().y);

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y + ImGui::GetWindowSize().y));
		ImGui::End();
	}

	ImGui::PopStyleColor();
}

XMVECTOR BossScene::SplinePosition(const std::vector<XMVECTOR>& posints, size_t startIndex, float t)
{
	size_t n = posints.size() - 2;

	if (startIndex > n)return posints[n];
	if (startIndex < 1)return posints[1];

	XMVECTOR p0 = posints[startIndex - 1];
	XMVECTOR p1 = posints[startIndex];
	XMVECTOR p2 = posints[startIndex + 1];
	XMVECTOR p3 = posints[startIndex + 2];

	XMVECTOR position = 0.5 * ((2 * p1 + (-p0 + p2) * t) +
		(2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
		(-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t);

	return position;
}

void BossScene::DamageEffect(UINT maxFrame, UINT nowFrame) {
	float rate = (float)nowFrame / (float)maxFrame;

	rate = 1 - rate;
	constexpr float  c4 = 2.f * XM_PI / 3.f;
	float easeRate = -powf(2.f, 10.f * rate - 10.f) * sinf((rate * 10.f - 10.75f) * c4);

	float shiftNum = easeRate * 0.1f;

	PostEffect::GetInstance()->SetShiftG({ shiftNum,0.f });
}