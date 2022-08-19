#include "Player.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Input.h"

Player::Player()
	:Player(ObjModel::LoadFromObj("rat"))
{
	obj->SetPosition({0,0,0});
	obj->SetScale({1,1,1});
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
	for (auto& i :bullet)
	{
		i.Draw();
	}

	obj->Draw();
}

void Player::Shot(ObjModel* model, float scale)
{
	bullet.emplace_back(model, obj->GetPosition());
	bullet.back().SetScale({ scale,scale,scale });
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
