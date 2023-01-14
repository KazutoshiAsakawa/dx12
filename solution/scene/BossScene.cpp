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

XMFLOAT3 lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t) {
	XMFLOAT3 ret;
	ret.x = a.x + t * (b.x - a.x);
	ret.y = a.y + t * (b.y - a.y);
	ret.z = a.z + t * (b.z - a.z);
	return ret;
}

void BossScene::Initialize(DirectXCommon* dxcommon) {
	// Input�̃C���X�^���X���擾
	Input* input = Input::GetInstance();

	// �}�E�X�J�[�\��������
	ShowCursor(false);

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/gameplay.png");
	SpriteCommon::GetInstance()->LoadTexture(2, L"Resources/aim.png");
	SpriteCommon::GetInstance()->LoadTexture(3, L"Resources/hp/hp.png");
	SpriteCommon::GetInstance()->LoadTexture(4, L"Resources/hp/hpSlide.png");

	SpriteCommon::GetInstance()->LoadTexture(6, L"Resources/pause/pauseBack.png");
	SpriteCommon::GetInstance()->LoadTexture(7, L"Resources/pause/pauseTitle.png");
	SpriteCommon::GetInstance()->LoadTexture(8, L"Resources/pause/pauseClose.png");

	SpriteCommon::GetInstance()->LoadTexture(9, L"Resources/operation/ESC_Pause.png");

	SpriteCommon::GetInstance()->LoadTexture(10, L"Resources/bossText.png");

	// �X�v���C�g�̐���
	aim.reset(Sprite::Create(2));
	aim->SetPosition({ WinApp::window_width / 2.f ,WinApp::window_height / 2.f, 0.f });

	operationSprite["ESC_Pause"].reset(Sprite::Create(9, { 0.f, 0.f }, false, false));
	// �ŏ��͕\������
	operationSprite["ESC_Pause"]->SetIsInvisible(false);

	// �|�[�Y��ʂ̉摜�����
	pauseSprite.resize(pauseMax);
	for (UINT i = 0; i < pauseMax; i++) {
		// hp�摜��texNumber�̍ŏ���6
		pauseSprite[i].reset(Sprite::Create(i + 6, { 0,0 }));
	}

	// �J�����̏�����
	camera.reset(new TrackingCamera());

	ObjObject3d::SetCamera(camera.get());

	// OBJ���烂�f���f�[�^��ǂݍ���

	// �X�J�C�h�[��
	skyDomeModel.reset(ObjModel::LoadFromObj("skydome"));
	skyDomeObj = ObjObject3d::Create();
	skyDomeObj->SetModel(skyDomeModel.get());
	skyDomeObj->SetScale({ 5,5,5 });

	// �n��
	groundModel.reset(ObjModel::LoadFromObj("ground"));
	groundModel->SetTiling({ 100.f,100.f });
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

#pragma region �v���C���[
	// �v���C���[������
	player = std::make_unique<Player>();

	player->SetHp(playerHpMax);

	// hp�摜
	playerHpSpriteSize = { 330.f, 30.f };
	playerHpSprite.reset(Sprite::Create(3, { 0,1 }));
	playerHpSprite->SetPosition(XMFLOAT3(35, WinApp::window_height - 35, 0));
	playerHpSprite->SetSize({ 0.f,playerHpSpriteSize.y });
	playerHpSlide.reset(Sprite::Create(4, { 0,1 }));
	playerHpSlide->SetPosition(XMFLOAT3(35, WinApp::window_height - 35, 0));
	playerHpSlide->SetSize(playerHpSpriteSize);

	playerHpSprite->TransferVertexBuffer();
	playerHpSprite->Update();
	playerHpSlide->TransferVertexBuffer();
	playerHpSlide->Update();

#pragma endregion �v���C���[

#pragma region �J����

	// �J�������v���C���[�̈ʒu�ɃZ�b�g
	camera->SetTrackingTarget(player.get());
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.z -= 500;
	eye.y += 50;
	camera->SetEye(eye);
	cameraLengthDef = camera->GetEyeToCameraTargetLength();

#pragma endregion �J����

#pragma region �{�X

	// �{�X
	boss.reset(new Boss(enemyModel.get(), { 0.f, 0.f, 50.f }));

	boss->SetPhaseApproach();
	boss->SetAttackTarget(player.get());
	boss->SetBulletModel(ObjModel::LoadFromObj("enemyBullet"));
	bossScale = 2.f;
	boss->SetScale({ bossScale,bossScale,bossScale });


	boss->SetAlive(false);

	bossEntryNowFrame = 0;

#pragma region �{�XHP

	// �{�X�̗̑�
	boss->SetHp(bossHpMax);

	// �{�XHP�X�v���C�g
	bossHpSpriteSize = { 430.f, 30.f };
	bossHpSprite.reset(Sprite::Create(3, { 0.5f, 0.f }));
	bossHpSlide.reset(Sprite::Create(4, { 0.5f, 0.f }));

	bossHpSprite->SetPosition({ (float)WinApp::window_width / 2.f, 5.f, 0.f });
	bossHpSlide->SetPosition({ (float)WinApp::window_width / 2.f, 5.f, 0.f });

	bossHpSprite->SetColor({ 0.5f, 1.f, 1.f, 1.f });

	bossHpSprite->SetSize({ 0.f, bossHpSpriteSize.y });
	bossHpSlide->SetSize(bossHpSpriteSize);

	bossHpSprite->TransferVertexBuffer();
	bossHpSlide->TransferVertexBuffer();

	bossHpSprite->Update();
	bossHpSlide->Update();

#pragma endregion �{�XHP

#pragma region �{�X���O

	// �{�X�̖��O�X�v���C�g
	bossText.reset(Sprite::Create(10, { 0.5f, 0.5f }));
	bossText->SetPosition({ (float)WinApp::window_width / 2.f, (float)WinApp::window_height / 4.f * 3.f, 0.f });

	constexpr float texSizeX = WinApp::window_width * (3.f / 4.f);	// �摜�����͉�ʉ�����3/4
	float texRate = bossText->GetTexSize().y / bossText->GetTexSize().x;	// �c�͉��̉��{��
	bossText->SetSize({ texSizeX, texSizeX * texRate });

	bossText->SetIsInvisible(true);

	bossText->TransferVertexBuffer();
	bossText->Update();

#pragma endregion �{�X���O

#pragma endregion �{�X

	// �p�[�e�B�N��������
	ParticleManager::GetInstance()->SetCamera(camera.get());

	// �X�V�����̊֐�������
	updateProcess = std::bind(&BossScene::start, this);

#pragma region ��

	// Resources/�ɑ����t�@�C���p�X
	bgmFileName = "sound/zensen he totugekiseyo.wav";

	// BGM�ǂݍ���
	Audio::GetInstance()->LoadWave(bgmFileName);
	// BGM���Đ�
	Audio::GetInstance()->PlayWave(bgmFileName, 0.06f, XAUDIO2_LOOP_INFINITE);

#pragma endregion ��

}

void BossScene::Finalize() {
	DamageEffect(1, 1);

	// �}�E�X�J�[�\�����o��
	ShowCursor(true);
}

void BossScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		bool invisibleFlag = operationSprite["ESC_Pause"]->GetIsInvisible();
		operationSprite["ESC_Pause"]->SetIsInvisible(!invisibleFlag);
	}

	if (operationSprite["ESC_Pause"]->GetIsInvisible() == false) {
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

		player->Update();

		boss->Update();

		groundObj->Update();
		skyDomeObj->Update();

		// �X�v���C�g�X�V
		aim->Update();
		operationSprite["ESC_Pause"]->Update();
	} else {
		pauseSprite[pause]->Update();
		if (Input::GetInstance()->TriggerKey(DIK_W)) {
			if (--pause <= -1) {
				pause = 2;
			}
		}
		if (Input::GetInstance()->TriggerKey(DIK_S)) {
			if (++pause >= 3) {
				pause = 0;
			}
		}

		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (pause == 0) {
				operationSprite["ESC_Pause"]->SetIsInvisible(false);
			} else if (pause == 1) {
				SceneManager::GetInstance()->ChangeScene("TITLE");
			} else if (pause == 2) {
				PostQuitMessage(0); //OS�ɑ΂��āA�A�v���̏I����`����
			}
		}
	}
}

// �V�[���J��
void BossScene::start() {
	// ���U�C�N�������鎞��
	constexpr UINT mosaicFrameMax = 120;

	// ���U�C�N�̎��Ԃ��ő�܂ł�������play�֐��ɕς���
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcess��bossEntry�֐����Z�b�g
		camera->SetTrackingTarget(boss.get());
		updateProcess = std::bind(&BossScene::bossEntry, this);
		mosaicFrame = 0;
		bossText->SetIsInvisible(false);
	} else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);

		rate *= rate * rate;

		constexpr XMFLOAT3 startEye = { 100,0,-100 };
		constexpr XMFLOAT3 endEye = { 0,0,0 };

		XMFLOAT3 eye;
		eye = lerp(startEye, endEye, rate);
		player->SetPosition(eye);

		constexpr XMFLOAT3 startRota = { 0.f, 360.f, 0.f };
		constexpr XMFLOAT3 endRota = { 0.f, 0.f, 0.f };

		XMFLOAT3 rota;
		rota = lerp(startRota, endRota, rate);
		player->SetRotation(rota);
	}
}

void BossScene::bossEntry() {
	constexpr UINT frameMax = 360;

	// �t���[�����ő�܂ōs�����玟�̊֐���
	if (bossEntryNowFrame++ > frameMax) {
		camera->SetTrackingTarget(player.get());
		updateProcess = std::bind(&BossScene::play, this);

		camera->SetEyeToCameraTargetLength(cameraLengthDef);

		boss->SetAlive(true);

		playerHpSprite->SetSize(playerHpSpriteSize);
		playerHpSprite->TransferVertexBuffer();
		playerHpSprite->Update();

		bossHpSprite->SetSize(bossHpSpriteSize);
		bossHpSprite->TransferVertexBuffer();
		bossHpSprite->Update();

		// �{�X�̖��O��\��
		bossText->SetIsInvisible(true);
		// �v���C���[�̏Ə���\��
		aim->SetIsInvisible(false);
	} else {
		// �i�s�x
		float rate = (float)bossEntryNowFrame / (float)frameMax;

		// �̗̓o�[�����񂾂񑝂₷
		playerHpSprite->SetSize({ playerHpSpriteSize.x * (1.f - powf(1.f - rate, 4.f)),
			playerHpSprite->GetSize().y });
		playerHpSprite->TransferVertexBuffer();
		playerHpSprite->Update();

		bossHpSprite->SetSize({ bossHpSpriteSize.x * (1.f - powf(1.f - rate, 4.f)),
			bossHpSprite->GetSize().y });
		bossHpSprite->TransferVertexBuffer();
		bossHpSprite->Update();


		// �C�[�W���O(4��)
		rate *= rate * rate * rate;

		// �J�����̋����̍ő�l
		const float lengthMax = cameraLengthDef * 2.f;

		// ���̃J�����̋���
		float cameraLength = cameraLengthDef + rate * (lengthMax - cameraLengthDef);

		// ���̃J�����̋������Z�b�g
		camera->SetEyeToCameraTargetLength(cameraLength);

		// �{�X�̖��O
		bossText->SetColor({ 1.f, 1.f, 1.f, 1.f - (rate * rate * rate * rate) });

		bossText->Update();

		// �v���C���[�̏Ə���\��
		aim->SetIsInvisible(true);
	}
}

void BossScene::play() {

	// �X�J�C�h�[���̈ʒu
	skyDomeObj->SetPosition(player->GetPosition());

	// �v���C���[�̈ړ��Ɖ��
	PlayerMove();

	// �v���C���[�̉�]
	{
		XMFLOAT3 playerRot = player->GetRotation();
		constexpr float rotSpeed = 0.1f;

		playerRot.y -= rotSpeed * mousePosDiff.x;
		playerRot.x -= rotSpeed * mousePosDiff.y;

		// �v���C���[��]����
		if (playerRot.x > 60.f) {
			playerRot.x = 60.f;
		}
		if (playerRot.x < -60.f) {
			playerRot.x = -60.f;
		}

		player->SetRotation(playerRot);
	}

	if (boss->GetAlive()) {
		// �Ə��ƓG�̃X�N���[�����W�̓����蔻��
		CollisionAimAndEnemyScreenPos();
	} else if (killBossFlag) {// �{�X�����񂾂�
		updateProcess = std::bind(&BossScene::end, this, "CLEAR");
	}

	// �G�Ǝ��@�̒e�̓����蔻��
	CollisionEnemyAndPlayerBullet();

	// ���@�ƓG�̒e�̓����蔻��
	CollisionPlayerAndEnemyBullet();
	
	// RGB���炵�̃t���O�Ǘ�
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

	if (player->GetHp() > 0) {

		// �X�v���C�g���� = �ő�l * hp�̊���
		playerHpSprite->SetSize(XMFLOAT2(playerHpSpriteSize.x * (float)player->GetHp() / playerHpMax,
			playerHpSprite->GetSize().y));

		playerHpSprite->TransferVertexBuffer();

		// �̗̓o�[
		playerHpSprite->Update();

		playerHpSlide->Update();
	}
	if (boss->GetHp() > 0) {

		// �X�v���C�g���� = �ő�l * hp�̊���
		bossHpSprite->SetSize(XMFLOAT2(bossHpSpriteSize.x * (float)boss->GetHp() / bossHpMax,
			bossHpSprite->GetSize().y));

		bossHpSprite->TransferVertexBuffer();
		bossHpSprite->Update();
		bossHpSlide->Update();
	}

	frame++;
}

void BossScene::killEffect() {
	constexpr UINT effectFrameMax = 180;

	if (++nowEffectFrame > effectFrameMax) {
		updateProcess = std::bind(&BossScene::end, this, "CLEAR");
	} else {
		// �p�[�e�B�N���̔���
		ParticleManager::GetInstance()->CreateParticle(boss->GetPosition(), 10, 1, 5);

		// �i�s�x
		float rate = (float)nowEffectFrame / (float)effectFrameMax;

		// �C�[�W���O(8��)
		rate *= rate * rate * rate * rate * rate * rate * rate;

		// 1����0�ɂ���
		rate = 1.f - rate;

		// �C�[�W���O�𔽉f�������{�X�̑傫��
		float scale = bossScale * rate;

		// �傫�����Z�b�g
		boss->SetScale({ scale, scale, scale });
	}
}

void BossScene::deathPlayer() {
	XMFLOAT3 rota = player->GetRotation();
	rota.z += 1.f;

	float scale = player->GetScale().z;
	scale -= 0.01f;

	if (rota.z > 90.f || scale < 0.f) {
		scale = 0.f;
		player->SetAlive(false);
		updateProcess = std::bind(&BossScene::end, this, "GAMEOVER");
	}
	player->SetRotation(rota);
	player->SetScale({ scale, scale, scale });

	// �n�ʂ܂ŗ�����
	XMFLOAT3 pos = player->GetPosition();
	if (pos.y > -4.f) {
		pos.y -= 0.1f;
		player->SetPosition(pos);
	}

}

void BossScene::end(const std::string& nextScene) {
	// ���U�C�N�������鎞��
	constexpr UINT mosaicFrameMax = 50;

	// ���U�C�N�̎��Ԃ��ő�܂ł�������play�֐��ɕς���
	if (++mosaicFrame > mosaicFrameMax) {

		// BGM�~�߂�
		Audio::GetInstance()->StopWave(bgmFileName);
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

void BossScene::Draw(DirectXCommon* dxcommon) {
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

	operationSprite["ESC_Pause"]->Draw();

	// �̗�
	if (player->GetHp() > 0) {
		playerHpSprite->Draw();
	}
	playerHpSlide->Draw();

	// �{�X�̗�
	if (boss->GetHp() > 0) {
		bossHpSprite->Draw();
	}
	bossHpSlide->Draw();

	bossText->Draw();

	aim->Draw();

	if (operationSprite["ESC_Pause"]->GetIsInvisible()) {
		pauseSprite[pause]->Draw();
	}
}

XMVECTOR BossScene::SplinePosition(const std::vector<XMVECTOR>& posints, size_t startIndex, float t) {
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

// �v���C���[�̈ړ��Ɖ��
void BossScene::PlayerMove() {
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

		if (hitW || hitS || hitA || hitD || hitZ || hitX || hitSpace) {
			auto pos = player->GetPosition();
			float moveSpeed = 0.2f;

			// ���
			if (hitSpace && avoidFrame == 0) {
				moveSpeed *= 10;
				// �N�[���^�C��
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
			if (hitS) {
				pos.x -= forward.x;
				pos.y -= forward.y;
				pos.z -= forward.z;
			}
			if (hitD) {
				pos.x += right.x;
				pos.y += right.y;
				pos.z += right.z;
			}
			if (hitA) {
				pos.x -= right.x;
				pos.y -= right.y;
				pos.z -= right.z;
			}

			// �ړ�����(�n��)
			if (0 > pos.y) {
				pos.y = 0;
			}

			player->SetPosition(pos);
		}
	}
}

// �Ə��ƓG�̃X�N���[�����W�̓����蔻��
void BossScene::CollisionAimAndEnemyScreenPos() {

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

	if (flag) {
		aim->SetColor({ 1,0,0,1 });
	} else {
		aim->SetColor({ 1,1,1,1 });
	}

	++shotInterval;

	// ���N���b�N���Ă�����
	if (input->PushMouse(Input::LEFT)) {
		// ���Ԋu�Œe���o��
		if (shotInterval >= shotIntervalMax) {
			shotInterval = 0;
			// ���@�̒e�̔���
			player->Shot(pBulletModel.get(), pBulletScale);
		}

	}
}

// �G�Ǝ��@�̒e�̓����蔻��
void BossScene::CollisionEnemyAndPlayerBullet() {
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
				boss->Damage(1);
				pb.SetAlive(false);
				aim->SetColor({ 1,1,1,1 });

				if (boss->GetHp() == 0) {
					boss->SetAlive(false);
					killBossFlag = true;

					camera->SetTrackingTarget(boss.get());
					updateProcess = std::bind(&BossScene::killEffect, this);
					XMFLOAT3 angle = camera->GetAngle();
					angle.y += 180.f;
					camera->SetAngle(angle);
				}

				// �p�[�e�B�N���̔���
				ParticleManager::GetInstance()->CreateParticle(boss->GetPosition(), 10, 4, 5, { 1,0,0 }, { 0.5f,0,0 });
			}
		}
	}
}

// ���@�ƓG�̒e�̓����蔻��
void BossScene::CollisionPlayerAndEnemyBullet() {
	{
		Sphere playerShape;

		playerShape.center = XMLoadFloat3(&player->GetPosition());
		playerShape.radius = player->GetScale().z;

		// �Փ˔��������
		if (player->GetAlive()) {
			for (auto& eb : boss->GetBullet()) {
				if (!eb.GetAlive())continue;
				Sphere eBulletShape;
				eBulletShape.center = XMLoadFloat3(&eb.GetPosition());
				// �G�e��Z���̑傫��
				eBulletShape.radius = eb.GetScale().z;

				// ���������������
				if (Collision::CheckSphere2Sphere(playerShape, eBulletShape)) {
					eb.SetAlive(false);				// �G�̒e������
					player->Damage(1);				// �v���C���[�Ƀ_���[�W
					shiftFlag = true;				// RGB���炵������
					nowFrame = 0;

					if (player->GetHp() == 0) {		// �̗͂�0�ɂȂ�����
						shiftFlag = false;	// RGB���炵�͂��Ȃ�
						// �v���C���[���S���o�ֈړ�
						updateProcess = std::bind(&BossScene::deathPlayer, this);
					}
				}
			}
		}
	}
}
