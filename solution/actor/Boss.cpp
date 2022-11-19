#include "Boss.h"

using namespace DirectX;

void Boss::Update()
{
	if (alive) {
		phase();

		// ���W�ϊ��̌v�Z
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

	// XMVECTOR�ɕϊ�
	XMVECTOR vectorVel = XMLoadFloat3(&vel);
	// �傫����1�ɂ���
	vectorVel = XMVector3Normalize(vectorVel);
	// �傫����C�ӂ̒l�ɂ���
	vectorVel = XMVectorScale(vectorVel, 0.1f);
	// FLOAT3�ɕϊ�
	XMStoreFloat3(&vel, vectorVel);

	XMFLOAT3 pos;
	pos = GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;
	SetPosition(pos);
}