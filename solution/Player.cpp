#include "Player.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Input.h"

Player::Player()
	: model(FbxLoader::GetInstance()->LoadModelFromFile("boneTest")),
	object(new FbxObject3d())
{
	object->Initialize();
	object->SetModel(model.get());

	object->SetPosition({ 0,0,0 });
}

Player::~Player()
{

}

void Player::Update()
{
	Move();
	object->Update();
}

void Player::Draw()
{
	object->Draw(DirectXCommon::GetInstance()->GetCmdList());
}

void Player::PlayAnimation() {
	object->PlayAnimation();
}

void Player::Move(float speed) {
	const bool hitW = Input::GetInstance()->PushKey(DIK_W);
	const bool hitS = Input::GetInstance()->PushKey(DIK_S);
	const bool hitA = Input::GetInstance()->PushKey(DIK_A);
	const bool hitD = Input::GetInstance()->PushKey(DIK_D);

	if (hitW || hitS || hitA || hitD) {
		auto pos = object->GetPosition();

		if (hitW) {
			pos.z += speed;
		}
		else if (hitS) {
			pos.z -= speed;
		}

		if (hitD) {
			pos.x += speed;
		}
		else if (hitA) {
			pos.x -= speed;
		}

		object->SetPosition(pos);
	}
}