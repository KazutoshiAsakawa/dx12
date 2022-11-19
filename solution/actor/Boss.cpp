#include "Boss.h"

using namespace DirectX;

void Boss::Update()
{
	if (alive) {
		phase();

		// 座標変換の計算
		Screen();
		obj->Update();


	}
}

void Boss::Draw()
{
	if (alive) {
		obj->Draw();
	}
}

void Boss::Approach()
{
	XMFLOAT3 vel;
	vel.x = attackTarget->GetPosition().x - GetPosition().x;
	vel.y = attackTarget->GetPosition().y - GetPosition().y;
	vel.z = attackTarget->GetPosition().z - GetPosition().z;

	// XMVECTORに変換
	XMVECTOR vectorVel = XMLoadFloat3(&vel);
	// 距離の二乗
	float lengthSq = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
	// 大きさを1にする
	vectorVel = XMVector3Normalize(vectorVel);
	// 大きさを任意の値にする
	vectorVel = XMVectorScale(vectorVel, 0.1f);
	// FLOAT3に変換
	XMStoreFloat3(&vel, vectorVel);

	XMFLOAT3 pos;
	pos = GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;
	SetPosition(pos);

	// 一定距離近づいたら離れる
	if (lengthSq < (GetScale().z * 3) * (GetScale().z * 3)) {
		SetPhase(std::bind(&Boss::Leave, this));
	}
}

void Boss::Leave()
{
	XMFLOAT3 vel;
	vel.x = attackTarget->GetPosition().x - GetPosition().x;
	vel.y = attackTarget->GetPosition().y - GetPosition().y;
	vel.z = attackTarget->GetPosition().z - GetPosition().z;

	// XMVECTORに変換
	XMVECTOR vectorVel = XMLoadFloat3(&vel);
	// 距離の二乗
	float lengthSq = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
	// 大きさを1にする
	vectorVel = XMVector3Normalize(vectorVel);
	// 大きさを任意の値にする
	vectorVel = XMVectorScale(vectorVel, -0.1f);
	// FLOAT3に変換
	XMStoreFloat3(&vel, vectorVel);

	XMFLOAT3 pos;
	pos = GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;
	SetPosition(pos);

	// 一定距離離れたら近づく
	if (lengthSq > (GetScale().z * 30) * (GetScale().z * 30)) {
		SetPhase(std::bind(&Boss::Approach, this));
	}
}
