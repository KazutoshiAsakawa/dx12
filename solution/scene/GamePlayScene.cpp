#include "GamePlayScene.h"
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

void GamePlayScene::Initialize(DirectXCommon* dxcommon)
{
	// �}�E�X�J�[�\��������
	ShowCursor(false);

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/gameplay.png");
	SpriteCommon::GetInstance()->LoadTexture(2, L"Resources/aim.png");
	SpriteCommon::GetInstance()->LoadTexture(3, L"Resources/hp/hp.png");
	SpriteCommon::GetInstance()->LoadTexture(4, L"Resources/hp/hpSlide.png");

	SpriteCommon::GetInstance()->LoadTexture(6, L"Resources/pouse/pauseBack.png");
	SpriteCommon::GetInstance()->LoadTexture(7, L"Resources/pouse/pauseTitle.png");
	SpriteCommon::GetInstance()->LoadTexture(8, L"Resources/pouse/pauseClose.png");

	SpriteCommon::GetInstance()->LoadTexture(9, L"Resources/operation/W.png");
	SpriteCommon::GetInstance()->LoadTexture(10, L"Resources/operation/S.png");
	SpriteCommon::GetInstance()->LoadTexture(11, L"Resources/operation/A.png");
	SpriteCommon::GetInstance()->LoadTexture(12, L"Resources/operation/D.png");
	SpriteCommon::GetInstance()->LoadTexture(13, L"Resources/operation/L_Click.png");
	SpriteCommon::GetInstance()->LoadTexture(14, L"Resources/operation/ESC_Pause.png");

	operationSprite["W"].reset(Sprite::Create(9, { 0.5f, 0.5f }, false, false));
	operationSprite["S"].reset(Sprite::Create(10, { 0.5f, 0.5f }, false, false));
	operationSprite["A"].reset(Sprite::Create(11, { 0.5f, 0.5f }, false, false));
	operationSprite["D"].reset(Sprite::Create(12, { 0.5f, 0.5f }, false, false));
	operationSprite["L_Click"].reset(Sprite::Create(13, { 0.f, 0.f }, false, false));
	operationSprite["ESC_Pause"].reset(Sprite::Create(14, { 0.f, 0.f }, false, false));
	// �ŏ��͕\������
	operationSprite["ESC_Pause"]->SetIsInvisible(false);

	operationSprite["W"]->SetPosition({ WinApp::window_width / 2.f,
										WinApp::window_height / 2.f + 50.f,
										0.f
		});
	operationSprite["S"]->SetPosition({ WinApp::window_width / 2.f,
										WinApp::window_height / 2.f + 225.f,
										0.f
		});
	operationSprite["A"]->SetPosition({ WinApp::window_width / 2.f - 100.f,
										WinApp::window_height / 2.f + 150.f,
										0.f
		});
	operationSprite["D"]->SetPosition({ WinApp::window_width / 2.f + 100.f,
										WinApp::window_height / 2.f + 150.f,
										0.f
		});

	// �X�v���C�g�̐���
	aim.reset(Sprite::Create(2));

	// hp�摜
	playerHpSprite.reset(Sprite::Create(3, { 0,1 }));
	playerHpSprite->SetPosition(XMFLOAT3(40, WinApp::window_height - 40, 0));

	playerHpSlide.reset(Sprite::Create(4, { 0,1 }));
	playerHpSlide->SetPosition(XMFLOAT3(35, WinApp::window_height - 35, 0));

	// �|�[�Y��ʂ̉摜�����
	pouseSprite.resize(pouseMax);
	for (UINT i = 0; i < pouseMax; i++) {
		// hp�摜��texNumber�̍ŏ���6
		pouseSprite[i].reset(Sprite::Create(i + 6, { 0,0 }));
	}

	// �J�����̏�����
	camera.reset(new TrackingCamera());

	ObjObject3d::SetCamera(camera.get());

	// ���[��
	lane.reset(new GameObject(nullptr));
	// �^�[�Q�b�g��������
	nullTarget.reset(new GameObject(nullptr));

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
	enemyModel.reset(ObjModel::LoadFromObj("butterfly"));// enemy2

	//�f�o�C�X���Z�b�g
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//�J�������Z�b�g
	FbxObject3d::SetCamera(camera.get());
	//�O���t�B�b�N�X�p�C�v���C������
	FbxObject3d::CreateGraphicsPipeline();
	// �v���C���[������
	player = std::make_unique<Player>();

	// �v���C���[�̐e��ݒ�
	player->SetParent(lane->GetObj());

	// �v���C���[�̗̑͂�ݒ�
	player->SetHp(playerHpMax);

	// �J���������[���̈ʒu�ɃZ�b�g
	camera->SetTrackingTarget(player.get());
	camera->SetTarget(lane->GetPosition());
	XMFLOAT3 eye = lane->GetPosition();
	eye.z -= 50;
	eye.y += 10;
	camera->SetEye(eye);

	// �G
	enemy.resize(0);

	// �p�[�e�B�N��������
	ParticleManager::GetInstance()->SetCamera(camera.get());

	// �X�V�����̊֐�������
	updateProcess = std::bind(&GamePlayScene::start, this);

	// �����ǂݍ���
	Audio::GetInstance()->LoadWave("Alarm01.wav");

	// �X�v���C���Ȑ�
	csv = Enemy::LoadCsv("Resources/spline.csv");

	// �X�^�[�g��2�K�v������ŏ��̍��W�����炩���ߓ���Ă���
	points.emplace_back(XMVectorSet(std::stof(csv[0][0]),
		std::stof(csv[0][1]),
		std::stof(csv[0][2]),
		1));

	// csv�̒��g���i�[
	for (auto& i : csv) {
		points.emplace_back(XMVectorSet(std::stof(i[0]),
			std::stof(i[1]),
			std::stof(i[2]),
			1));
	}

	// �G���h��2�K�v
	points.emplace_back(points.back());

	splineStartIndex = 1;

	// ����
	toriiModel.reset(ObjModel::LoadFromObj("torii"));
	toriiObj.resize(points.size());

	for (UINT i = 0; i < toriiObj.size(); i++) {
		toriiObj[i].resize(2);
		for (UINT x = 0; x < toriiObj[i].size(); x++) {
			toriiObj[i][x] = ObjObject3d::Create();
			toriiObj[i][x]->Initialize();
			toriiObj[i][x]->SetModel(toriiModel.get());
			toriiObj[i][x]->SetScale(XMFLOAT3(4, 4, 4));
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, points[i]);
			// pos.y -= 15.f;
			pos.y = -5.f;

			toriiObj[i][x]->SetPosition(pos);
		}
	}

	// ��
	wallModel.reset(ObjModel::LoadFromObj("fence"));// wall
	wallObj.resize(points.size());

	for (UINT i = 0; i < wallObj.size(); i++) {
		wallObj[i].resize(2);
		for (UINT x = 0; x < wallObj[i].size(); x++) {
			wallObj[i][x] = ObjObject3d::Create();
			wallObj[i][x]->Initialize();
			wallObj[i][x]->SetModel(wallModel.get());
			wallObj[i][x]->SetScale(XMFLOAT3(4, 4, 50));
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, points[i]);
			pos.x -= 40.f;
			pos.x += 80.f * (float)x;
			pos.y = -5.f;
			wallObj[i][x]->SetPosition(pos);
		}
	}

	// CSV�ǂݍ���
	csv = Enemy::LoadCsv("Resources/enemy.csv");

	{
		UINT nowframe = 0;

		for (auto& y : csv) {
			if (y[0].find("//") == 0) {
				continue;
			}
			if (y[0] == "POPPOSITION") {
				// �G���o��
				enemyPos.emplace_back(std::stof(y[1]), std::stof(y[2]), std::stof(y[3]));
			}
			else if (y[0] == "POPTIME") {
				// �w�莞��
				nowframe += std::stoul(y[1]);
				enemyFrame.emplace_back(nowframe, nowframe + 60 * 39);// (�G���o�鎞��,�G�������鎞��)
			}
		}
	}
}

void GamePlayScene::Finalize()
{
	DamageEffect(1, 1);

	// �}�E�X�J�[�\�����o��
	ShowCursor(true);
}

void GamePlayScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		bool invisibleFlag = operationSprite["ESC_Pause"]->GetIsInvisible();
		operationSprite["ESC_Pause"]->SetIsInvisible(!invisibleFlag);
	}

	if (operationSprite["ESC_Pause"]->GetIsInvisible() == false) {
		// �V�[���J��
		updateProcess();

		// �p�[�e�B�N���X�V
		ParticleManager::GetInstance()->Update();

		camera->Update();
		lane->Update();

		//fbxObj->Update();
		player->Update();
		//}
		for (auto& i : enemy) {
			i->Update();
		}

		for (auto& i : toriiObj) {
			for (auto& x : i) {
				x->Update();
			}
		}

		for (auto& i : wallObj) {
			for (auto& x : i) {
				x->Update();
			}
		}

		groundObj->Update();
		skyDomeObj->Update();

		// �X�v���C�g�X�V
		aim->SetPosition({ (float)Input::GetInstance()->GetMousePos().x,(float)Input::GetInstance()->GetMousePos().y,0 });
		aim->Update();

		if (player->GetHp() > 0) {

			// �X�v���C�g���� = �ő�l * hp�̊���
			playerHpSprite->SetSize(XMFLOAT2(playerHpSprite->GetTexSize().x * (float)player->GetHp() / playerHpMax,
				playerHpSprite->GetSize().y));

			playerHpSprite->TransferVertexBuffer();

			// �̗̓o�[
			playerHpSprite->Update();

			playerHpSlide->Update();

			for (auto& i : operationSprite)
			{
				i.second->Update();
			}
		}
	}// �|�[�Y���
	else {
		pouseSprite[pouse]->Update();
		if (Input::GetInstance()->TriggerKey(DIK_W)) {
			if (--pouse <= -1) {
				pouse = 2;
			}
		}
		if (Input::GetInstance()->TriggerKey(DIK_S)) {
			if (++pouse >= 3) {
				pouse = 0;
			}
		}

		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (pouse == 0) {
				operationSprite["ESC_Pause"]->SetIsInvisible(false);
			}
			else if (pouse == 1) {
				SceneManager::GetInstance()->ChangeScene("TITLE");
			}
			else if (pouse == 2) {
				PostQuitMessage(0); //OS�ɑ΂��āA�A�v���̏I����`����
			}
		}
	}
}

// �V�[���J��
void GamePlayScene::start()
{
	Input* input = Input::GetInstance();
	// ���U�C�N�������鎞��
	constexpr UINT mosaicFrameMax = 50;

	// ���U�C�N�̎��Ԃ��ő�܂ł�������play�֐��ɕς���
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcess��play�֐����Z�b�g
		updateProcess = std::bind(&GamePlayScene::play, this);
		mosaicFrame = 0;
	}
	else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void GamePlayScene::play()
{
	Input* input = Input::GetInstance();

	skyDomeObj->SetPosition(lane->GetPosition());

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
			XMFLOAT3 rot{};

			if (hitSpace && avoidFrame == 0) {
				moveSpeed *= 10;
				avoidFrame = avoidFrameMax;
			}

			if (hitW && pos.y < 8.f) {
				pos.y += moveSpeed;
				rot.x -= 4.f;
				operationSprite["W"]->SetIsInvisible(true);

			}
			else if (hitS && pos.y > -4.f) {
				pos.y -= moveSpeed;
				rot.x += 4.f;
				operationSprite["S"]->SetIsInvisible(true);
			}

			if (hitD && pos.x < 10.f) {
				pos.x += moveSpeed;
				rot.z -= 4.f;
				operationSprite["D"]->SetIsInvisible(true);
			}
			else if (hitA && pos.x > -10.f) {
				pos.x -= moveSpeed;
				rot.z += 4.f;
				operationSprite["A"]->SetIsInvisible(true);
			}

			player->SetPosition(pos);
			player->SetRotation(rot);
		}
	}

	// �������
	{
		if (operationSprite["L_Click"]->GetIsInvisible() == false) {
			operationSprite["L_Click"]->SetPosition({ aim->GetPosition().x, aim->GetPosition().y, 0.f });
		}
	}

	// �G�𔭐�
	for (auto& i : enemyFrame) {
		if (i.first <= frame) {// �G�̑��x��ݒ�
			auto& a = EnemyAdd(enemyPos[addedEnemyNum], { 0.f, 0.f, -0.25f });
			addedEnemyNum++;
			a->SetLifeSpan(i.second);
		}
	}

	enemyFrame.remove_if([&](std::pair<UINT, UINT>& i) {return i.first <= frame; });

	// �G���S�����Ȃ��Ȃ�����G���h�V�[���ɍs��
	{
		if (enemyFrame.empty() && enemy.empty()) {

			updateProcess = std::bind(&GamePlayScene::end, this);
		}
	}


	for (auto& i : enemy) {
		// �G��Z��0�ɍs������s���p�^�[����leave�ɕς���
		if (i->GetPosition().z < 0) {
			i->LeaveChange(XMFLOAT3(0.5f, 0.5f, 0.f));
		}

		// z����0���Wy����10�ȏ�(��ʊO)�ɍs���������
		if (i->GetPosition().z < 0.f && i->GetPosition().y > 10.f) {
			i->SetAlive(false);
		}

		// x����13�ȏゾ�����獶�ɍs��
		if (i->GetPosition().x >= 13.f) {
			i->DirectionChange(XMFLOAT3(-0.1f, 0.f, 0.f));
		}

		// x����-13�ȉ���������E�ɍs��
		if (i->GetPosition().x <= -13.f) {
			i->DirectionChange(XMFLOAT3(0.4f, 0.f, 0.f));
		}
	}
	enemy.remove_if([&](std::unique_ptr<Enemy>& i) {return i->GetLifeSpan() <= frame; });

	// �G��������Ə��ƓG�̓����蔻�������
	if (!enemy.empty())
	{
		// �摜�̍���ƉE��
		XMFLOAT2 aimLT = { (float)input->GetMousePos().x - aim->GetSize().x / 2,
		(float)input->GetMousePos().y - aim->GetSize().y / 2 };

		XMFLOAT2 aimRB = { (float)input->GetMousePos().x + aim->GetSize().x / 2,
		(float)input->GetMousePos().y + aim->GetSize().y / 2 };

		// �G�̏ꏊ
		XMFLOAT2 enemyPos;

		bool flag = false;

		// �Ə��ƓG�̃X�N���[�����W�̓����蔻��
		player->SetShotTarget(nullptr);
		for (auto& i : enemy) {
			if (!i->GetAlive())continue;
			enemyPos = { i->GetFloat2ScreenPos().x,i->GetFloat2ScreenPos().y };

			// �����蔻��
			if (aimLT.x <= enemyPos.x && aimLT.y <= enemyPos.y &&
				aimRB.x >= enemyPos.x && aimRB.y >= enemyPos.y) {
				flag = true;

				player->SetShotTarget(i.get());
			}
		}

		if (flag) {
			aim->SetColor({ 1,0,0,1 });
			// �Ə��������Ă����獶�N���b�N
			if (input->TriggerMouse(Input::LEFT)) {
				// ���@�̒e�̔���
				player->Shot(pBulletModel.get(), pBulletScale);

				XMFLOAT3 pos = lane->GetPosition();
				pos.x += player->GetPosition().x;
				pos.y += player->GetPosition().y;
				pos.z += player->GetPosition().z;

				// �������������
				operationSprite["L_Click"]->SetIsInvisible(true);
			}
		}
		else {
			// �Ə�����
			aim->SetColor({ 1,1,1,1 });
		}
	}


	// �G�Ǝ��@�̒e�̓����蔻��
	{
		Sphere pBulletShape;

		for (auto& pb : player->GetBullet()) {
			if (!pb.GetAlive())continue;
			pBulletShape.center = XMLoadFloat3(&pb.GetPosition());
			pBulletShape.radius = pb.GetScale().x;

			// �Փ˔��������
			for (auto& e : enemy) {
				if (!e->GetAlive())continue;
				Sphere enemyShape;
				enemyShape.center = XMLoadFloat3(&e->GetPosition());
				enemyShape.radius = e->GetScale().x;

				// ����������
				if (Collision::CheckSphere2Sphere(pBulletShape, enemyShape)) {
					pb.SetAlive(false);

					e->Damage(1);				// �G�Ƀ_���[�W

					if (e->GetHp() <= 0) {		// �̗͂�0�ȉ��ɂȂ�����
						e->SetAlive(false);

						// �p�[�e�B�N���̏ꏊ��ݒ�
						XMFLOAT3 pos = lane->GetPosition();
						pos.x += e->GetPosition().x;
						pos.y += e->GetPosition().y;
						pos.z += e->GetPosition().z;

						// �p�[�e�B�N���̔���
						ParticleManager::GetInstance()->CreateParticle(pos, 100, 4, 10);
					}

					// �p�[�e�B�N���̏ꏊ��ݒ�
					XMFLOAT3 pos = lane->GetPosition();
					pos.x += e->GetPosition().x;
					pos.y += e->GetPosition().y;
					pos.z += e->GetPosition().z;

					// �U��
					e->SetShake(true);
					// �q�b�g�X�g�b�v
					e->SetHitStop(true);

					// �p�[�e�B�N���̔���
					ParticleManager::GetInstance()->CreateParticle(pos, 10, 4, 5);
					break;
				}
			}
		}
		// �G������
		enemy.erase(std::remove_if(enemy.begin(), enemy.end(),
			[](const std::unique_ptr <Enemy>& i) {return !i->GetAlive() && i->GetBullet().empty(); }), enemy.end());
	}

	// �G�̒e�Ǝ��@
	{
		Sphere playerShape;

		playerShape.center = XMLoadFloat3(&player->GetPosition());
		playerShape.radius = player->GetScale().z;

		// �Փ˔��������
		if (player->GetAlive()) {
			for (auto& e : enemy) {
				for (auto& eb : e->GetBullet()) {
					Sphere eBulletShape;
					eBulletShape.center = XMLoadFloat3(&eb.GetPosition());
					eBulletShape.radius = eb.GetScale().z;

					// ���������������
					if (Collision::CheckSphere2Sphere(playerShape, eBulletShape)) {
						eb.SetAlive(false);				// �G�̒e������
						player->Damage(1);				// �v���C���[�Ƀ_���[�W
						shiftFlag = true;				// RGB���炵������
						shiftNowFrame = 0;
						if (player->GetHp() == 0) {		// �̗͂�0�ɂȂ�����
							player->SetAlive(false);

							SceneManager::GetInstance()->ChangeScene("GAMEOVER");
						}
						break;
					}
				}
			}
		}
	}

#ifdef _DEBUG
	// �{�X�V�[���ɍs��
	if (input->TriggerKey(DIK_F)) {
		updateProcess = std::bind(&GamePlayScene::end, this);
	}
#endif //_DEBUG

	if (shiftFlag) {

		DamageEffect(shiftMaxFrame, shiftNowFrame);
		shiftNowFrame++;
		if (shiftMaxFrame < shiftNowFrame) {
			shiftFlag = false;
			shiftNowFrame = 0;
		}
	}

	// �X�v���C���Ȑ��ňړ�
	{
		splineFrame++;
		float timeRate = (float)splineFrame / 120.f;
		if (timeRate >= 1.0f)
		{
			if (splineStartIndex < points.size() - 3) {
				splineStartIndex++;
				timeRate -= 1.0f;
				splineFrame = 0;
			}
			else
			{
				timeRate = 1.0f;
			}
		}
		// �x�N�^�[���t���[�g�ɕϊ�
		XMFLOAT3 splineFloat;
		XMStoreFloat3(&splineFloat, SplinePosition(points, splineStartIndex, timeRate));
		lane->SetPosition(splineFloat);
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

void GamePlayScene::end()
{
	Input* input = Input::GetInstance();
	// ���U�C�N�������鎞��
	constexpr UINT mosaicFrameMax = 50;

	// ���U�C�N�̎��Ԃ��ő�܂ł�������play�֐��ɕς���
	if (++mosaicFrame > mosaicFrameMax) {
		//PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// �{�X�V�[����
		SceneManager::GetInstance()->ChangeScene("BOSSPLAY");
	}
	else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		rate = 1 - rate;// 1����0
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void GamePlayScene::Draw(DirectXCommon* dxcommon)
{
	// 3D�I�u�W�F�N�g�`��O����
	ObjObject3d::PreDraw();

	for (auto& i : toriiObj) {
		for (auto& x : i) {
			x->Draw();
		}
	}

	for (auto& i : wallObj) {
		for (auto& x : i) {
			x->Draw();
		}
	}

	// �n��
	groundObj->Draw();

	//�X�J�C�h�[���̕`��
	skyDomeObj->Draw();

	// �v���C���[�̕`��
	player->Draw();
	// �G�̕����`��
	for (auto& i : enemy) {
		i->Draw();
	}
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

void GamePlayScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();

	// �̗�
	if (player->GetHp() > 0) {
		playerHpSprite->Draw();
	}
	playerHpSlide->Draw();

	for (auto& i : operationSprite)
	{
		i.second->Draw();
	}

	aim->Draw();

	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TitleBgActive, ImVec4(0.5f, 0.125f, 0.125f, 1.f));

	if (operationSprite["ESC_Pause"]->GetIsInvisible()) {
		// �|�[�Y��ʕ`��
		pouseSprite[pouse]->Draw();
	}
	else {
		ImGui::SetNextWindowSize(ImVec2(100, 200));
		ImGui::Begin(u8"����", nullptr, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text(u8"�X�y�[�X:���");

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y + ImGui::GetWindowSize().y));
		ImGui::End();
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

XMVECTOR GamePlayScene::SplinePosition(const std::vector<XMVECTOR>& posints, size_t startIndex, float t)
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

std::unique_ptr<Enemy>& GamePlayScene::EnemyAdd(XMFLOAT3 pos, XMFLOAT3 vel)
{
	enemy.emplace_back();
	auto& e = enemy.back();

	// �G�̏������W
	e = std::make_unique<Enemy>(enemyModel.get(), pos);
	// �G�̑傫��
	e->SetScale(XMFLOAT3(enemyScale, enemyScale, enemyScale));
	// �G�̑��x
	e->SetVel(vel);
	// �G�̕W�I
	e->SetShotTarget(player.get());

	e->SetParent(lane->GetObj());

	return e;
}

void GamePlayScene::DamageEffect(UINT maxFrame, UINT nowFrame) {
	float rate = (float)nowFrame / (float)maxFrame;

	rate = 1 - rate;
	constexpr float  c4 = 2.f * XM_PI / 3.f;
	float easeRate = -powf(2, 10.f * rate - 10.f) * sinf((rate * 10.f - 10.75f) * c4);

	float shiftNum = easeRate * 0.1f;

	PostEffect::GetInstance()->SetShiftG({ shiftNum,0.f });
}