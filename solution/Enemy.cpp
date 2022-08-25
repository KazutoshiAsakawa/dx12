#include "Enemy.h"

#include <fstream>
#include <sstream>

using namespace DirectX;

Enemy::Enemy(ObjModel* model, const DirectX::XMFLOAT3& position)
	:GameObject(model, position), phase(std::bind(&Enemy::approach, this)) {
	// Shot(ObjModel::LoadFromObj("rat"),1);
	bulletModel.reset(ObjModel::LoadFromObj("enemyBullet"));
	nowShotFrame = shotInterval;
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

void Enemy::Update()
{
	if (alive) {
		phase();
	}

	for (auto& i : bullet) {
		i.Update();
	}

	// 敵の弾を消
	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](EnemyBullet& i) {return !i.GetAlive(); }), bullet.end());

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
	bullet.back().SetScale({ scale,scale,scale });
	XMFLOAT3 vel = { 0,0,0.3 };
	// XMStoreFloat3(&vel, XMVector3Transform(XMVectorSet(0, 0, 1, 1), obj->GetMatRot()));

	bullet.back().SetVel(vel);
}

void Enemy::approach()
{

	XMFLOAT3 pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	obj->SetPosition(pos);
	obj->Update();

	if (pos.z < 0) {
		vel = XMFLOAT3(0.5, 0.5, 0);
		phase = std::bind(&Enemy::leave, this);
	}

	if (nowShotFrame-- == 0u) {
		Shot(bulletModel.get(), 1);
		nowShotFrame = shotInterval;
	}
}

void Enemy::leave()
{

	XMFLOAT3 pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	obj->SetPosition(pos);
	obj->Update();

	if (pos.x > 10) {
		alive = false;
	}
}
