#include "Enemy.h"

#include <fstream>
#include <sstream>
#include <random>
#include "WinApp.h"

using namespace DirectX;

namespace {

	// 球面線形補間
	XMVECTOR SLerp(XMVECTOR vel, XMVECTOR vec, float t) {
		// 大きさを1にする
		XMVECTOR normVel = XMVector3Normalize(vel);
		XMVECTOR normVec = XMVector3Normalize(vec);
		// velとvecの成す角
		float angle = 0.f;
		XMStoreFloat(&angle, XMVector3Dot(normVel, normVec));

		float sinAngle = acos(sin(angle));
		float sinAngleFrom = sin((1 - t) * angle);
		float sinAngleTo = sin(t * angle);
		// 長さを計算
		float a = 0.f;
		XMStoreFloat(&a, XMVector3Length(vel));
		float b = 0.f;
		XMStoreFloat(&b, XMVector3Length(vec));

		// a + t * (b - a);
		// ベクトルの大きさ
		float lerpVal = a + t * (b - a);
		// 向き
		XMVECTOR slerpVec = (sinAngleFrom * normVel + sinAngleTo * normVec) / sinAngle;
		return lerpVal * slerpVec;
	}
}

Enemy::Enemy(ObjModel* model, const DirectX::XMFLOAT3& position)
	:GameObject(model, position), phase(std::bind(&Enemy::PhaseApproach, this)) {
	bulletModel.reset(ObjModel::LoadFromObj("enemyBullet"));
	bulletScale = 1.f;

	nowShotFrame = shotInterval;

	hp = 2;
}

// CSV読み込み
std::vector<std::vector<std::string>> Enemy::LoadCsv(const std::string& FilePath) {
	std::vector<std::vector<std::string>> csvData{};

	// ファイルを開く
	std::ifstream ifs(FilePath);
	if (!ifs) {
		return csvData;
	}

	std::string line{};
	// 一行ずつ読み込む
	for (UINT i = 0; std::getline(ifs, line); i++) {
		csvData.emplace_back();

		std::istringstream stream(line);
		std::string field;
		// 読み込んだ行を','区切りで分割
		while (std::getline(stream, field, ',')) {
			csvData[i].emplace_back(field);
		}
	}
	return csvData;
}

void Enemy::Update() {
	if (alive) {
		phase();
	}

	// 振動
	if (shakeFlag) {
		Shake();
	}

	if (hitStopFlag) {
		hitStop();
	}

	// 誘導弾
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

			// 一定の角度に行ったら追わない
			angleVec = XMVector3AngleBetweenNormals(vel, vec);
			if (XMVectorGetX(angleVec) > XM_PI / 3.f) continue;

			vel = SLerp(vel, vec, 0.02f);
			// 速度の大きさを指定
			vel *= 0.8f;

			// XMStoreFloat3(&XMFLOAT3の変数, XMVECTORの変数);
			XMStoreFloat3(&floatVel, vel);
			i.SetVel(floatVel);

			// 標的に向ける
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

	// 敵の弾を消す
	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](EnemyBullet& i) {return !i.GetAlive(); }), bullet.end());

	// obj->SetRotation({0,180,0});

	// 座標変換の計算
	Screen();
	obj->Update();
}

void Enemy::Draw() {
	for (auto& i : bullet) {
		i.Draw();
	}

	if (alive) {
		obj->Draw();
	}
}

void Enemy::Shot(ObjModel* model, float scale) {
	bullet.emplace_back(model, obj->GetPosition());
	bullet.back().SetScale({ scale ,scale ,scale });
	bullet.back().SetParent(obj->GetParent());
	XMFLOAT3 vel = { 0.f,0.f,-0.8f };

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
		// 大きさを1にする
		vectorVel = XMVector3Normalize(vectorVel);
		// 大きさを任意の値にする
		vectorVel = XMVectorScale(vectorVel, 0.8f);
		// FLOAT3に変換
		XMStoreFloat3(&vel, vectorVel);

		// 標的に向ける
		float rotx = atan2f(vel.y, vel.z);
		float roty = atan2f(vel.x, vel.z);
		bullet.back().SetRotation(XMFLOAT3(XMConvertToDegrees(rotx), XMConvertToDegrees(roty), 0));
	}

	bullet.back().SetVel(vel);
}

void Enemy::Shake() {
	if (shakeRate >= 0) {
		XMFLOAT3 pos = obj->GetPosition();
		// memoryPos = pos;

		pos.x = WinApp::GetInstance()->MyRand(memoryPos.x, 0.5f * shakeRate);
		pos.y = WinApp::GetInstance()->MyRand(memoryPos.y, 0.5f * shakeRate);

		shakeRate -= 0.025f;
		obj->SetPosition(pos);
	} else {
		shakeFlag = false;
		shakeRate = shakeRateDef;
		obj->SetPosition(memoryPos);
	}
}

void Enemy::hitStop() {
	// 最初のフレームだけ読み込む、速度を下げる
	if (hitStopFrame == hitStopFrameDef) {
		memoryVel = vel;

		vel = { 0.f,0.f ,0.f };
	}

	// カウント
	if (hitStopFrame > 0) {
		hitStopFrame--;
	} else {// カウントし終わったら速度を戻す
		hitStopFrame = hitStopFrameDef;
		hitStopFlag = false;

		vel = memoryVel;
	}

}

void Enemy::PhaseApproach() {

	XMFLOAT3 pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	memoryPos.x += vel.x;
	memoryPos.y += vel.y;
	memoryPos.z += vel.z;

	obj->SetPosition(pos);
	obj->Update();

	// 0になったら撃つ
	if (nowShotFrame-- == 0) {
		Shot(bulletModel.get(), bulletScale);
		// 初期化
		nowShotFrame = shotInterval;
	}
}

void Enemy::PhaseLeave() {
	XMFLOAT3 pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	memoryPos.x += vel.x;
	memoryPos.y += vel.y;
	memoryPos.z += vel.z;

	obj->SetPosition(pos);
	obj->Update();
}

void Enemy::LeaveChange(XMFLOAT3 vel) {
	SetVel(vel);

	// leaveに変える
	phase = std::bind(&Enemy::PhaseLeave, this);
}

void Enemy::Direction() {
	XMFLOAT3 pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	obj->SetPosition(pos);
	obj->Update();

	// 0になったら撃つ
	if (nowShotFrame-- == 0) {
		Shot(bulletModel.get(), bulletScale);
		// 初期化
		nowShotFrame = shotInterval;
	}
}

void Enemy::DirectionChange(XMFLOAT3 vel) {
	SetVel(vel);

	// leaveに変える
	phase = std::bind(&Enemy::Direction, this);
}