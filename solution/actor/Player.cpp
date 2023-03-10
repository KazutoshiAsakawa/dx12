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

	for (auto& i : bullet)
	{
		i.Update();
	}

	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](PlayerBullet& i) {return !i.GetAlive(); }), bullet.end());

	obj->Update();
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
	bullet.back().SetIsBillboard(true);
	XMFLOAT3 vel;
	constexpr float velScale = 2.f;

	XMStoreFloat3(&vel, XMVector3Transform(XMVectorSet(0, 0, velScale, 1), obj->GetMatRot()));

	// 標的の方向へ飛ばす
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
	}

	bullet.back().SetVel(vel);
}
