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
	// �����̓��
	float lengthSq = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
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

	// ��苗���߂Â����痣���
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

	// XMVECTOR�ɕϊ�
	XMVECTOR vectorVel = XMLoadFloat3(&vel);
	// �����̓��
	float lengthSq = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
	// �傫����1�ɂ���
	vectorVel = XMVector3Normalize(vectorVel);
	// �傫����C�ӂ̒l�ɂ���
	vectorVel = XMVectorScale(vectorVel, -0.1f);
	// FLOAT3�ɕϊ�
	XMStoreFloat3(&vel, vectorVel);

	XMFLOAT3 pos;
	pos = GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;
	SetPosition(pos);

	// ��苗�����ꂽ��߂Â�
	if (lengthSq > (GetScale().z * 30) * (GetScale().z * 30)) {
		SetPhase(std::bind(&Boss::Approach, this));
	}
}
