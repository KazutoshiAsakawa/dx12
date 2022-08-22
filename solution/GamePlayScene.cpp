#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Collision.h"

#include "PostEffect.h"

using namespace DirectX;

void GamePlayScene::Initialize(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/gameplay.png");
	// spriteCommon->LoadTexture(2, L"Resources/house.png");

	// �X�v���C�g�̐���
	sprite = Sprite::Create(1, { 0,0 }, false, false);

	//for (int i = 0; i < 20; i++)// 20��������Ƃ�
	//{
	//	int texNumber = 1;
	//	// �X�v���C�g�̌��_������ɂȂ�
	//	// sprites[i] = SpriteCreate(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height, texNumber, spriteCommon, { 0,0 }, false, false);
	//	Sprite* sprite = Sprite::Create(spriteCommon, texNumber, { 0,0 }, false, false);

	//	// �X�v���C�g�̍��W�ύX
	//	sprite->SetPosition({ (float)(rand() % 1280),(float)(rand() % 720),0 });
	//	// sprite->SetRotation((float)(rand() % 360));
	//	// sprite->SetSize({ (float)(rand() % 400) ,(float)(rand() % 400) });
	//	//sprites[i].isInvisible = true;

	//	//sprites[i].size.x = 400.0f;
	//	//sprites[i].size.y = 100.0f;
	//	// ���_�o�b�t�@�ɔ��f
	//	sprite->TransferVertexBuffer();
	//	sprites.push_back(sprite);
	//}

	camera.reset(new DebugCamera(WinApp::window_width, WinApp::window_height));
	//camera->Initialize(WinApp::window_width, WinApp::window_height);
	// camera = new DebugCamera(WinApp::window_width, WinApp::window_height);

	ObjObject3d::SetCamera(camera.get());

	// OBJ���烂�f���f�[�^��ǂݍ���
	// model_1 = Model::LoadFromObj("ground");
	model = ObjModel::LoadFromObj("triangle_mat");
	// 3D�I�u�W�F�N�g����

	object3d = ObjObject3d::Create();
	// object3d.reset(Object3d::Create());

	object3d->SetModel(model);

	pBulletModel.reset(ObjModel::LoadFromObj("sphere"));
	enemyModel.reset(ObjModel::LoadFromObj("sphere"));

	// 3D�I�u�W�F�N�g��3D���f�����Ђ��Â�
	//object3d_1->SetModel(model_1);

	// Fbx�ǂݍ���
	//fbxModel = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	//�f�o�C�X���Z�b�g
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//�J�������Z�b�g
	FbxObject3d::SetCamera(camera.get());
	//�O���t�B�b�N�X�p�C�v���C������
	FbxObject3d::CreateGraphicsPipeline();

	//fbxObj = new FbxObject3d();
	//fbxObj->Initialize();
	//fbxObj->SetModel(fbxModel);
	//fbxObj->SetPosition({ 0,0,0 });

	player = std::make_unique<Player>();

	constexpr UINT enemyNum = 1;
	enemy.resize(enemyNum);
	for (auto& i : enemy) {
		i = std::make_unique<Enemy>(enemyModel.get(), XMFLOAT3(0, 0, 30));
		i->SetScale(XMFLOAT3(enemyScale, enemyScale, enemyScale));
		i->SetVel(XMFLOAT3(0,0,-0.1));
	}

	// �����ǂݍ���
	Audio::GetInstance()->LoadWave("Alarm01.wav");

	// �����Đ�
	// audio->PlayWave("Alarm01.wav");
}

void GamePlayScene::Finalize()
{
	// �X�v���C�g�ʉ��
		/*for (auto& sprite : sprites) {
			delete sprite;
		}*/
	delete sprite;

	// delete fbxModel;
	//delete fbxObj;

	// sprites.clear();

	// 3D���f�����
	delete model;
}

void GamePlayScene::Update()
{
	Input* input = Input::GetInstance();

	// ���͂̊m�F
	//if (input->TriggerKey(DIK_0)) // ������0�L�[��������Ă�����
	//{
	//	OutputDebugStringA("Hit 0\n");  // �o�̓E�B���h�E�ɁuHit 0�v�ƕ\��
	//}

	//if (input->Triggerkey(DIK_SPACE))     // �X�y�[�X�L�[��������Ă�����
	//{
	//    // ��ʃN���A�J���[�̐��l������������
	//    clearColor[1] = 1.0f;
	//}

	// ���W����
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
		XMFLOAT3 playerRot = player->GetRotation();
		if (input->PushKey(DIK_RIGHT)) {
			playerRot.y += 1.f;
		}
		if (input->PushKey(DIK_LEFT)) {
			playerRot.y -= 1.f;
		}

		if (input->PushKey(DIK_UP)) {
			playerRot.x += 1.f;
		}
		if (input->PushKey(DIK_DOWN)) {
			playerRot.x -= 1.f;
		}

		player->SetRotation(playerRot);
	}

	if (input->TriggerKey(DIK_D) || input->TriggerKey(DIK_A))
	{

	}

	if (input->TriggerKey(DIK_0)) {
		player->Shot(pBulletModel.get(), pBulletScale);

	}

	{
		Sphere pBulletShape;

		for (auto& pb : player->GetBullet()) {
			if (!pb.GetAlive())continue;
			pBulletShape.center = XMLoadFloat3(&pb.GetPos());
			pBulletShape.radius = pb.GetScale().x;

			// �Փ˔��������
			for (auto& e : enemy) {
				Sphere enemyShape;
				enemyShape.center = XMLoadFloat3(&e->GetPos());
				enemyShape.radius = e->GetScale().x;

				// ����������
				if (Collision::CheckSphere2Sphere(pBulletShape, enemyShape)) {
					e->SetAlive(false);
					pb.SetAlive(false);
					break;
				}
			}
		}
		enemy.erase(std::remove_if(enemy.begin(), enemy.end(), [](const std::unique_ptr <Enemy>& i) {return !i->GetAlive(); }), enemy.end());
	}

	// �V�[���؂�ւ�
	if (input->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("END");
	}

	// X���W,Y���W���w�肵�ĕ\��
	DebugText::GetInstance()->Print("Hello,DirectX!!", 200, 100);
	// X���W,Y���W,�k�ڂ��w�肵�ĕ\��
	DebugText::GetInstance()->Print("Nihon Kogakuin", 200, 200, 2.0f);

	//sprite.rotation = 45;
	//sprite.position = {1280/2, 720/2, 0};
	//sprite.color = {0, 0, 1, 1};

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

	// �J�����ړ�
	//{
	//	const bool hitW = input->PushKey(DIK_W);
	//	const bool hitS = input->PushKey(DIK_S);
	//	const bool hitA = input->PushKey(DIK_A);
	//	const bool hitD = input->PushKey(DIK_D);

	//	DirectX::XMFLOAT3 camMoveVal{};
	//	constexpr float camMoveLen = 0.1f;

	//	if (hitW || hitS) {
	//		if (hitW) {
	//			camMoveVal.z += camMoveLen;
	//		}
	//		else if (hitS) {
	//			camMoveVal.z -= camMoveLen;
	//		}
	//	}

	//	if (hitA || hitD) {
	//		if (hitA) {
	//			camMoveVal.x -= camMoveLen;
	//		}
	//		else if (hitD) {
	//			camMoveVal.x += camMoveLen;
	//		}
	//	}

	//	camera->MoveEyeVector(camMoveVal);
	//	DirectX::XMFLOAT3 camTarget = camera->GetTarget();
	//	camTarget.x += camMoveVal.x;
	//	camTarget.y += camMoveVal.y;
	//	camTarget.z += camMoveVal.z;
	//	camera->SetTarget(camTarget);

	camera->Update();
	//fbxObj->Update();
	player->Update();
	//}
	for (auto& i : enemy) {
		i->Update();
	}

	// 3D�I�u�W�F�N�g�X�V
	object3d->Update();

	// �X�v���C�g�X�V

	sprite->Update();
	/*for (auto& sprite : sprites) {
		sprite->Update();
	}*/
}

void GamePlayScene::Draw(DirectXCommon* dxcommon)
{
	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();
	// �X�v���C�g�`��
	/*for (auto& sprite : sprites) {
		sprite->Draw();
	}*/
	sprite->Draw();

	// 3D�I�u�W�F�N�g�`��O����
	ObjObject3d::PreDraw();
	// 3D�I�u�W�F�N�g�̕`��
	// object3d->Draw();

	//fbxObj->Draw(dxcommon->GetCmdList());
	player->Draw();
	for (auto& i : enemy) {
		i->Draw();
	}
	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	ObjObject3d::PostDraw();
	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();
}

void GamePlayScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();

}