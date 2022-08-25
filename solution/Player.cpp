#include "Player.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Input.h"

using namespace DirectX;

Player::Player()
	:Player(ObjModel::LoadFromObj("player"))
{
	obj->SetPosition({ 0,0,0 });
	obj->SetScale({ 1,1,1 });
}

void Player::Update()
{
	Move();

	for (auto& i : bullet)
	{
		i.Update();
	}

	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](PlayerBullet& i) {return !i.GetAlive(); }), bullet.end());

	obj->Update();
}

void Player::Draw()
{
	for (auto& i : bullet)
	{
		i.Draw();
	}

	if (alive) {
		obj->Draw();
	}
}

void Player::Shot(ObjModel* model, float scale)
{
	bullet.emplace_back(model, obj->GetPosition());
	bullet.back().SetScale({ scale,scale,scale });
	XMFLOAT3 vel;
	XMStoreFloat3(&vel, XMVector3Transform(XMVectorSet(0, 0, 1, 1), obj->GetMatRot()));
	bullet.back().SetVel(vel);
}

void Player::Move(float speed) {
	const bool hitW = Input::GetInstance()->PushKey(DIK_W);
	const bool hitS = Input::GetInstance()->PushKey(DIK_S);
	const bool hitA = Input::GetInstance()->PushKey(DIK_A);
	const bool hitD = Input::GetInstance()->PushKey(DIK_D);

	if (hitW || hitS || hitA || hitD) {
		auto pos = obj->GetPosition();

		if (hitW) {
			pos.y += speed;
		}
		else if (hitS) {
			pos.y -= speed;
		}

		if (hitD) {
			pos.x += speed;
		}
		else if (hitA) {
			pos.x -= speed;
		}

		obj->SetPosition(pos);
	}
}

//void Player::Bullet()
//{
//	const bool hitSpace = Input::GetInstance()->PushKey(DIK_SPACE);
//
//	if (hitSpace) {
//
//	}
//}
