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

	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](EnemyBullet& i) {return !i.GetAlive(); }), bullet.end());

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

void Boss::PhaseApproach()
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
	vectorVel = XMVectorScale(vectorVel, 0.3f);
	// FLOAT3�ɕϊ�
	XMStoreFloat3(&vel, vectorVel);

	XMFLOAT3 pos;
	pos = GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;
	SetPosition(pos);

	// ��苗���߂Â����痣���
	if (lengthSq < (GetScale().z * 19) * (GetScale().z * 19)) {
		SetPhase(std::bind(&Boss::PhaseLeave, this));
	}
}

void Boss::PhaseLeave()
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
	vectorVel = XMVectorScale(vectorVel, -0.3f);
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
		SetPhase(std::bind(&Boss::PhaseAttack, this));
		SetShotTarget(attackTarget);
		nowShotFrame = shotInterval;
	}
}

void Boss::PhaseAttack()
{
	// 0�ɂȂ����猂��
	if (nowShotFrame-- == 0) {
		Shot(bulletModel, 1);
		nowShotNum++;

		// ������
		nowShotFrame = shotInterval;

		if (nowShotNum >= shotNum) {
			SetPhase(std::bind(&Boss::PhaseSpreadAttack, this));
			nowShotNum = 0;
		}
	}
}

void Boss::PhaseSpreadAttack()
{
	if (nowShotFrame-- == 0) {
		meleeAttack();
		nowShotNum++;

		if (nowShotNum >= shotNum) {
			SetPhase(std::bind(&Boss::PhaseApproach, this));
			nowShotNum = 0;
		}
		// ������
		nowShotFrame = shotInterval;
	}
}

void Boss::meleeAttack()
{
	// �g�U��
	constexpr int  num = 10;

	for (int i = 0; i <= num; i++) {
		spreadBullet(bulletModel, 1, -(float)i / num * XM_PI);
	}
}

void Boss::spreadBullet(ObjModel* model, float scale, float angle)
{
	bullet.emplace_back(model, obj->GetPosition());
	bullet.back().SetScale({ scale / 3,scale / 3 ,scale });
	bullet.back().SetParent(obj->GetParent());
	XMFLOAT3 vel = { 0.f,0.f,-0.4f };

	/*vel.x = cos(angle) * 0.4f;
	vel.z = sin(angle) * 0.4f;*/

	// �ݒ肳��Ă�����
	if (shotTarget != nullptr) {
		// ���x���v�Z
		// ��������W�I�܂ł̃x�N�g��
		vel = {
			shotTarget->GetPosition().x - obj->GetPosition().x,
			shotTarget->GetPosition().y - obj->GetPosition().y,
			shotTarget->GetPosition().z - obj->GetPosition().z
		};
		// XMVECTOR�ɕϊ�
		XMVECTOR vectorVel = XMLoadFloat3(&vel);

		XMVECTOR rot = XMQuaternionRotationRollPitchYaw(0, angle + XM_PIDIV2, 0);
		vectorVel = XMVector3Rotate(vectorVel, rot);

		// �傫����1�ɂ���
		vectorVel = XMVector3Normalize(vectorVel);
		// �傫����C�ӂ̒l�ɂ���
		vectorVel = XMVectorScale(vectorVel, 0.4f);
		// FLOAT3�ɕϊ�
		XMStoreFloat3(&vel, vectorVel);

	}

	bullet.back().SetVel(vel);
}
