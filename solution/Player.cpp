#include "Player.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Input.h"

using namespace DirectX;

Player::Player()
	:Player(ObjModel::LoadFromObj("rat"))
{
	obj->SetPosition({ 0,0,0 });
	obj->SetScale({ 1,1,1 });

	aim = ObjObject3d::Create();
	aim->Initialize();
	aim->SetModel(ObjModel::LoadFromObj("sphere"));
	constexpr float aimLength = 10;
	aim->SetPosition({ 0,0,aimLength });
	aim->SetScale({ 0.5,0.5,1 });
	aim->SetParent(obj.get());

	shotTarget = nullptr;
}

void Player::Update()
{
	// 移動処理
	Move();

	{
		XMMATRIX mat =
			obj->GetCamera()->GetViewMatrix() *
			obj->GetCamera()->GetProjectionMatrix() *
			obj->GetCamera()->GetViewPortMatrix();

		XMVECTOR screenAimPos = XMVector3Transform(aim->GetMatWorld().r[3], mat);
		screenAimPos /= XMVectorGetW(screenAimPos);

		// 変数に格納
		float2ScreenAimPos = XMFLOAT2(XMVectorGetX(screenAimPos), XMVectorGetY(screenAimPos));
	}

	for (auto& i : bullet)
	{
		i.Update();
	}

	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](PlayerBullet& i) {return !i.GetAlive(); }), bullet.end());

	obj->Update();
	aim->Update();
}

void Player::Draw()
{
	for (auto& i : bullet)
	{
		i.Draw();
	}

	if (alive) {
		obj->Draw();
		aim->Draw();
	}

}

void Player::Shot(ObjModel* model, float scale)
{
	bullet.emplace_back(model, obj->GetPosition());
	bullet.back().SetScale({ scale,scale,scale });
	bullet.back().SetParent(obj->GetParent());
	XMFLOAT3 vel;
	constexpr float velScale = 2.f;

	XMStoreFloat3(&vel, XMVector3Transform(XMVectorSet(0, 0, velScale, 1), obj->GetMatRot()));

	if (shotTarget != nullptr) {
		// 速度を計算
		// 自分から標的までのベクトル
		vel = {
			shotTarget->GetPos().x - obj->GetPosition().x,
			shotTarget->GetPos().y - obj->GetPosition().y,
			shotTarget->GetPos().z - obj->GetPosition().z
		};
		// XMVECTORに変換
		XMVECTOR vectorVel = XMLoadFloat3(&vel);
		// 大きさを1にする
		vectorVel = XMVector3Normalize(vectorVel);
		// 大きさを任意の値にする
		vectorVel = XMVectorScale(vectorVel, velScale);
		// FLOAT3に変換
		XMStoreFloat3(&vel, vectorVel);

		// 標的に向ける
		float rotx = atan2f(vel.y, vel.z);
		float roty = atan2f(vel.x, vel.z);
		bullet.back().SetRotation(XMFLOAT3(XMConvertToDegrees(rotx), XMConvertToDegrees(roty), 0));
	}

	bullet.back().SetVel(vel);
}

void Player::Move(float speed) {
	const bool hitW = Input::GetInstance()->PushKey(DIK_W);
	const bool hitS = Input::GetInstance()->PushKey(DIK_S);
	const bool hitA = Input::GetInstance()->PushKey(DIK_A);
	const bool hitD = Input::GetInstance()->PushKey(DIK_D);
	const bool hitZ = Input::GetInstance()->PushKey(DIK_Z);
	const bool hitX = Input::GetInstance()->PushKey(DIK_X);

	if (hitW || hitS || hitA || hitD || hitZ || hitX) {
		auto pos = obj->GetPosition();

		if (hitW && pos.y < 8.f) {
			pos.y += speed;
		}
		else if (hitS && pos.y > -4.f) {
			pos.y -= speed;
		}

		if (hitD && pos.x < 20.f) {
			pos.x += speed;
		}
		else if (hitA && pos.x > -20.f) {
			pos.x -= speed;
		}

		if (hitZ) {
			pos.z += speed;
		}
		else if (hitX) {
			pos.z -= speed;
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
