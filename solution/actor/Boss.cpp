#include "Boss.h"

using namespace DirectX;

void Boss::Update()
{
	if (alive) {
		phase();

		// À•W•ÏŠ·‚ÌŒvŽZ
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

	// XMVECTOR‚É•ÏŠ·
	XMVECTOR vectorVel = XMLoadFloat3(&vel);
	// ‘å‚«‚³‚ð1‚É‚·‚é
	vectorVel = XMVector3Normalize(vectorVel);
	// ‘å‚«‚³‚ð”CˆÓ‚Ì’l‚É‚·‚é
	vectorVel = XMVectorScale(vectorVel, 0.1f);
	// FLOAT3‚É•ÏŠ·
	XMStoreFloat3(&vel, vectorVel);

	XMFLOAT3 pos;
	pos = GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;
	SetPosition(pos);
}