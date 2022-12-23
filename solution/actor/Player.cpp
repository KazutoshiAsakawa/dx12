#include "Player.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Input.h"

using namespace DirectX;

Player::Player()
	:Player(ObjModel::LoadFromObj("fox"))
{
	// プレイヤーのオブジェクト
	obj->SetPosition({ 0,0,0 });
	obj->SetScale({ 1,1,1 });

	// 照準のオブジェクト
	aim = ObjObject3d::Create();
	aim->Initialize();
	constexpr float aimLength = 10.f;
	aim->SetPosition({ 0,0,aimLength });
	aim->SetScale({ 0.5,0.5,1 });
	aim->SetParent(obj.get());

	// 紅葉のオブジェクト
	momijiObj = ObjObject3d::Create();
	momijiObj->Initialize();
	// モデルをセット
	momijiObj->SetModel(ObjModel::LoadFromObj("MomijiLeave"));
	// 位置
	momijiObj->SetPosition({ 0.f, -obj->GetScale().y, 0.f });
	// 大きさ
	constexpr float momijiScale = 2.f;
	momijiObj->SetScale({ momijiScale, momijiScale, momijiScale });
	// 回転
	momijiObj->SetRotation({ 0.f, 180.f, 0.f });
	// 本体を親とする
	momijiObj->SetParent(obj.get());

	// 攻撃対象へのポインタ
	shotTarget = nullptr;

	// 体力を初期化
	hp = 1;
}

void Player::Update()
{
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
	aim->Update(); // プレイヤーの目の前にあるただのモデル
	momijiObj->Update();
}

void Player::Draw()
{
	for (auto& i : bullet)
	{
		i.Draw();
	}

	if (alive) {
		obj->Draw();
		momijiObj->Draw();
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
			shotTarget->GetPosition().x - obj->GetPosition().x,
			shotTarget->GetPosition().y - obj->GetPosition().y,
			shotTarget->GetPosition().z - obj->GetPosition().z
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
