#include "Boss.h"

using namespace DirectX;

void Boss::Update()
{
	// ボスの向き
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

		// XMStoreFloat3(&XMFLOAT3の変数, XMVECTORの変数);
		XMStoreFloat3(&floatVel, vel);

		// 標的に向ける
		float rotx = atan2f(-floatVel.y,
			sqrtf(floatVel.x * floatVel.x + floatVel.z * floatVel.z));
		float roty = atan2f(floatVel.x, floatVel.z);

		SetRotation(XMFLOAT3(XMConvertToDegrees(rotx), XMConvertToDegrees(roty) + 180, 0));
	}

	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](EnemyBullet& i) {return !i.GetAlive(); }), bullet.end());

	if (alive) {
		phase();

		// 座標変換の計算
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

	// XMVECTORに変換
	XMVECTOR vectorVel = XMLoadFloat3(&vel);
	// 距離の二乗
	float lengthSq = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
	// 大きさを1にする
	vectorVel = XMVector3Normalize(vectorVel);
	// 大きさを任意の値にする
	vectorVel = XMVectorScale(vectorVel, 0.3f);
	// FLOAT3に変換
	XMStoreFloat3(&vel, vectorVel);

	XMFLOAT3 pos;
	pos = GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;
	SetPosition(pos);

	// 一定距離近づいたら離れる
	if (lengthSq < (GetScale().z * 19) * (GetScale().z * 19)) {
		SetPhase(std::bind(&Boss::PhaseLeave, this));
	}
}

void Boss::PhaseLeave()
{
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
	vectorVel = XMVectorScale(vectorVel, -0.3f);
	// FLOAT3に変換
	XMStoreFloat3(&vel, vectorVel);

	XMFLOAT3 pos;
	pos = GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	// 移動制限(地面)
	if (0 > pos.y) {
		pos.y = 0;
	}

	SetPosition(pos);

	// 一定距離離れたら近づく
	if (lengthSq > (GetScale().z * 30) * (GetScale().z * 30)) {
		SetPhase(std::bind(&Boss::PhaseAttack, this));
		SetShotTarget(attackTarget);
		nowShotFrame = shotInterval;
	}
}

void Boss::PhaseAttack()
{
	// 0になったら撃つ
	if (nowShotFrame-- == 0) {
		Shot(bulletModel, 1);
		nowShotNum++;

		// 初期化
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
		// 初期化
		nowShotFrame = shotInterval;
	}
}

void Boss::meleeAttack()
{
	// 拡散率
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

	// 設定されていたら
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

		XMVECTOR rot = XMQuaternionRotationRollPitchYaw(0, angle + XM_PIDIV2, 0);
		vectorVel = XMVector3Rotate(vectorVel, rot);

		// 大きさを1にする
		vectorVel = XMVector3Normalize(vectorVel);
		// 大きさを任意の値にする
		vectorVel = XMVectorScale(vectorVel, 0.4f);
		// FLOAT3に変換
		XMStoreFloat3(&vel, vectorVel);

	}

	bullet.back().SetVel(vel);
}
