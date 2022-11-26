#include "Boss.h"

using namespace DirectX;

void Boss::Update()
{
	// �{�X�̌���
	{
		XMVECTOR vel, vec;
		XMFLOAT3 floatVel;

		vel = XMVectorSet(GetVel().x, GetVel().y, GetVel().z, 1);
		vel = XMVector3Normalize(vel);

		vec = XMVectorSet(
			attackTarget->GetPosition().x - GetPosition().x,
			attackTarget->GetPosition().y - GetPosition().y,
			attackTarget->GetPosition().z - GetPosition().z,
			1
		);
		vec = XMVector3Normalize(vec);

		// XMStoreFloat3(&XMFLOAT3�̕ϐ�, XMVECTOR�̕ϐ�);
		XMStoreFloat3(&floatVel, vel);

		// �W�I�Ɍ�����
		float rotx = atan2f(-floatVel.y,
			sqrtf(floatVel.x * floatVel.x + floatVel.z * floatVel.z));
		float roty = atan2f(floatVel.x, floatVel.z);

		SetRotation(XMFLOAT3(XMConvertToDegrees(rotx), XMConvertToDegrees(roty) + 180, 0));

	}

	if (alive) {
		phase();

		// ���W�ϊ��̌v�Z
		Screen();
		obj->Update();
	}

	for (auto& i : bullet) {
		i.Update();
	}
}

void Boss::Draw()
{
	if (alive) {
		obj->Draw();
	}

	for (auto& i : bullet) {
		i.Draw();
	}
}

void Boss::Approach()
{
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
	if (lengthSq < (GetScale().z * 12) * (GetScale().z * 12)) {
		SetPhase(std::bind(&Boss::Leave, this));
	}
}

void Boss::Leave()
{
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

	// �ړ�����(�n��)
	if (0 > pos.y) {
		pos.y = 0;
	}

	SetPosition(pos);

	// ��苗�����ꂽ��߂Â�
	if (lengthSq > (GetScale().z * 30) * (GetScale().z * 30)) {
		SetPhase(std::bind(&Boss::Attack, this));
		SetShotTarget(attackTarget);
		nowShotFrame = shotInterval;
		nowShotNum = 0;
	}
}

void Boss::Attack()
{
	// 0�ɂȂ����猂��
	if (nowShotFrame-- == 0) {
		Shot(bulletModel, 1);
		nowShotNum++;

		if (nowShotNum >= shotNum) {
			SetPhase(std::bind(&Boss::Approach, this));
		}

		// ������
		nowShotFrame = shotInterval;
	}
}
