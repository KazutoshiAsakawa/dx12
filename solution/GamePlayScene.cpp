#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

void GamePlayScene::Initialize()
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

	camera.reset(new Camera());
	camera->Initialize(WinApp::window_width, WinApp::window_height);

	ObjObject3d::SetCamera(camera.get());

	// OBJ���烂�f���f�[�^��ǂݍ���
	// model_1 = Model::LoadFromObj("ground");
	model = ObjModel::LoadFromObj("triangle_mat");
	// 3D�I�u�W�F�N�g����

	object3d = ObjObject3d::Create();
	// object3d.reset(Object3d::Create());

	object3d->SetModel(model);

	// 3D�I�u�W�F�N�g��3D���f�����Ђ��Â�
	//object3d_1->SetModel(model_1);

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

	// sprites.clear();

	// 3D���f�����
	delete model;
}

void GamePlayScene::Update()
{
	Input* input = Input::GetInstance();

	// ���͂̊m�F
	if (input->TriggerKey(DIK_0)) // ������0�L�[��������Ă�����
	{
		OutputDebugStringA("Hit 0\n");  // �o�̓E�B���h�E�ɁuHit 0�v�ƕ\��
	}

	//if (input->Triggerkey(DIK_SPACE))     // �X�y�[�X�L�[��������Ă�����
	//{
	//    // ��ʃN���A�J���[�̐��l������������
	//    clearColor[1] = 1.0f;
	//}

	// ���W����
	if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_RIGHT) || input->TriggerKey(DIK_LEFT))
	{

	}

	if (input->TriggerKey(DIK_D) || input->TriggerKey(DIK_A))
	{

	}

	// �V�[���؂�ւ�
	if (input->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	// X���W,Y���W���w�肵�ĕ\��
	DebugText::GetInstance()->Print("Hello,DirectX!!", 200, 100);
	// X���W,Y���W,�k�ڂ��w�肵�ĕ\��
	DebugText::GetInstance()->Print("Nihon Kogakuin", 200, 200, 2.0f);

	//sprite.rotation = 45;
	//sprite.position = {1280/2, 720/2, 0};
	//sprite.color = {0, 0, 1, 1};

	// �J�����ړ�
	{
		const bool hitW = input->PushKey(DIK_W);
		const bool hitS = input->PushKey(DIK_S);
		const bool hitA = input->PushKey(DIK_A);
		const bool hitD = input->PushKey(DIK_D);

		DirectX::XMFLOAT3 camMoveVal{};
		constexpr float camMoveLen = 0.1f;

		if(hitW || hitS) {
			if(hitW){
				camMoveVal.z += camMoveLen;
			} else if (hitS) {
				camMoveVal.z -= camMoveLen;
			}
		}

		if(hitA || hitD) {
			if(hitA) {
				camMoveVal.x -= camMoveLen;
			}else if(hitD){
				camMoveVal.x += camMoveLen;
			}
		}

		camera->MoveEyeVector(camMoveVal);
		DirectX::XMFLOAT3 camTarget = camera->GetTarget();
		camTarget.x += camMoveVal.x;
		camTarget.y += camMoveVal.y;
		camTarget.z += camMoveVal.z;
		camera->SetTarget(camTarget);

		camera->Update();
	}

	// 3D�I�u�W�F�N�g�X�V
	object3d->Update();

	// �X�v���C�g�X�V

	sprite->Update();
	/*for (auto& sprite : sprites) {
		sprite->Update();
	}*/
}

void GamePlayScene::Draw()
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
	object3d->Draw();

	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	ObjObject3d::PostDraw();
	// �X�v���C�g���ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();
}
