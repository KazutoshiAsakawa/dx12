#include "Enemy.h"

#include <fstream>
#include <sstream>

using namespace DirectX;

namespace {
	// ���ʐ��`���
	XMVECTOR SLerp(XMVECTOR vel, XMVECTOR vec, float t) {
		// �傫����1�ɂ���
		XMVECTOR normVel = XMVector3Normalize(vel);
		XMVECTOR normVec = XMVector3Normalize(vec);
		// vel��vec�̐����p
		float angle = 0.f;
		XMStoreFloat(&angle, XMVector3Dot(normVel, normVec));

		float sinAngle = acos(sin(angle));
		float sinAngleFrom = sin((1 - t) * angle);
		float sinAngleTo = sin(t * angle);
		// �������v�Z
		float a = 0.f;
		XMStoreFloat(&a, XMVector3Length(vel));
		float b = 0.f;
		XMStoreFloat(&b, XMVector3Length(vec));

		// a + t * (b - a);
		// �x�N�g���̑傫��
		float lerpVal = a + t * (b - a);
		// ����
		XMVECTOR slerpVec = (sinAngleFrom * normVel + sinAngleTo * normVec) / sinAngle;
		return lerpVal * slerpVec;
	}
}

Enemy::Enemy(ObjModel* model, const DirectX::XMFLOAT3& position)
	:GameObject(model, position), phase(std::bind(&Enemy::Approach, this)) {
	// Shot(ObjModel::LoadFromObj("rat"),1);
	bulletModel.reset(ObjModel::LoadFromObj("enemyBullet"));
	nowShotFrame = shotInterval;

	hp = 2;
}

// CSV�ǂݍ���
std::vector<std::vector<std::string>> Enemy::LoadCsv(const std::string& FilePath) {
	std::vector<std::vector<std::string>> csvData{};

	// �t�@�C�����J��
	std::ifstream ifs(FilePath);
	if (!ifs) {
		return csvData;
	}

	std::string line{};
	// ��s���ǂݍ���
	for (UINT i = 0; std::getline(ifs, line); i++) {
		csvData.emplace_back();

		std::istringstream stream(line);
		std::string field;
		// �ǂݍ��񂾍s��','��؂�ŕ���
		while (std::getline(stream, field, ',')) {
			csvData[i].emplace_back(field);
		}
	}
	return csvData;
}

void Enemy::Update()
{
	if (alive) {
		phase();
	}

	if (shakeFlag) {
		Shake();
	}

	// �U���e
	{
		XMVECTOR vel, vec;
		XMFLOAT3 floatVel;
		XMVECTOR angleVec;

		for (auto& i : bullet) {
			vel = XMVectorSet(i.GetVel().x, i.GetVel().y, i.GetVel().z, 1);
			vel = XMVector3Normalize(vel);

			vec = XMVectorSet(
				shotTarget->GetPosition().x - i.GetPosition().x,
				shotTarget->GetPosition().y - i.GetPosition().y,
				shotTarget->GetPosition().z - i.GetPosition().z,
				1
			);
			vec = XMVector3Normalize(vec);

			// ���̊p�x�ɍs������ǂ�Ȃ�
			angleVec = XMVector3AngleBetweenNormals(vel, vec);
			if (XMVectorGetX(angleVec) > XM_PI / 3.f) continue;

			vel = SLerp(vel, vec, 0.03f);
			// ���x�̑傫�����w��
			vel *= 0.8f;

			// XMStoreFloat3(&XMFLOAT3�̕ϐ�, XMVECTOR�̕ϐ�);
			XMStoreFloat3(&floatVel, vel);
			i.SetVel(floatVel);

			// �W�I�Ɍ�����
			float rotx = atan2f(-floatVel.y,
				sqrtf(floatVel.x * floatVel.x + floatVel.z * floatVel.z));
			float roty = atan2f(floatVel.x, floatVel.z);

			i.SetRotation(XMFLOAT3(XMConvertToDegrees(rotx), XMConvertToDegrees(roty), 0));
		}
	}

	for (auto& i : bullet) {
		i.Update();
	}

	frame++;

	// �G�̒e������
	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](EnemyBullet& i) {return !i.GetAlive(); }), bullet.end());

	Screen();
	obj->Update();
}

void Enemy::Draw()
{
	for (auto& i : bullet) {
		i.Draw();
	}

	if (alive) {
		obj->Draw();
	}
}

void Enemy::Shot(ObjModel* model, float scale)
{
	bullet.emplace_back(model, obj->GetPosition());
	bullet.back().SetScale({ scale / 3,scale / 3 ,scale });
	bullet.back().SetParent(obj->GetParent());
	XMFLOAT3 vel = { 0.f,0.f,-0.8f };

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
		// �傫����1�ɂ���
		vectorVel = XMVector3Normalize(vectorVel);
		// �傫����C�ӂ̒l�ɂ���
		vectorVel = XMVectorScale(vectorVel, 0.8f);
		// FLOAT3�ɕϊ�
		XMStoreFloat3(&vel, vectorVel);

		// �W�I�Ɍ�����
		float rotx = atan2f(vel.y, vel.z);
		float roty = atan2f(vel.x, vel.z);
		bullet.back().SetRotation(XMFLOAT3(XMConvertToDegrees(rotx), XMConvertToDegrees(roty), 0));
	}

	bullet.back().SetVel(vel);
}

void Enemy::Shake()
{
	if (shakeFrame >= 0) {
		XMFLOAT3 pos = obj->GetPosition();
		memoryPos = pos;

		pos.x = pos.x + (float)rand() / RAND_MAX * shakeFrame - shakeFrame / 2;
		pos.y = pos.y + (float)rand() / RAND_MAX * shakeFrame - shakeFrame / 2;

		shakeFrame -= 0.025f;
		obj->SetPosition(pos);
	}
	else {
		shakeFlag = false;
		shakeFrame = shakeFrameDef;
		obj->SetPosition(memoryPos);
	}
}

void Enemy::Approach()
{

	XMFLOAT3 pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	obj->SetPosition(pos);
	obj->Update();

	// 0�ɂȂ����猂��
	if (nowShotFrame-- == 0) {
		Shot(bulletModel.get(), 1);
		// ������
		nowShotFrame = shotInterval;
	}
}

void Enemy::Leave()
{
	XMFLOAT3 pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	obj->SetPosition(pos);
	obj->Update();
}

void Enemy::LeaveChange(XMFLOAT3 vel) {
	SetVel(vel);

	// leave�ɕς���
	phase = std::bind(&Enemy::Leave, this);
}

void Enemy::Direction() {
	XMFLOAT3 pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	obj->SetPosition(pos);
	obj->Update();

	// 0�ɂȂ����猂��
	if (nowShotFrame-- == 0) {
		Shot(bulletModel.get(), 1);
		// ������
		nowShotFrame = shotInterval;
	}
}

void Enemy::DirectionChange(XMFLOAT3 vel) {
	SetVel(vel);

	// leave�ɕς���
	phase = std::bind(&Enemy::Direction, this);
}