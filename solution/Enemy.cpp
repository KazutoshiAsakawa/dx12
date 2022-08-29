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

	frame++;

	// 敵の弾を消す
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
	bullet.back().SetScale({ scale / 3,scale / 3 ,scale });
	XMFLOAT3 vel;

	// 標的が設定されていないとき
	if (shotTarget == nullptr) {
		vel = { 0,0,-0.3 };
	}
	else {// 設定されていたら
		// 速度を計算
		// 自分か標的までのベクトル
		vel = {
			shotTarget->GetPos().x - obj->GetPosition().x,
			shotTarget->GetPos().y - obj->GetPosition().y,
			shotTarget->GetPos().z - obj->GetPosition().z
		};
		// XMVECTORに変換
		XMVECTOR vectorVel = XMLoadFloat3(&vel);
		// 大きさを1にする
		vectorVel = XMVector3Normalize(vectorVel);
		// 大きさを任意の値にする
		vectorVel = XMVectorScale(vectorVel, 0.3);
		// FLOAT3に変換
		XMStoreFloat3(&vel, vectorVel);

		// 標的に向ける
		float rotx = atan2f(vel.y, vel.z);
		float roty = atan2f(vel.x, vel.z);
		bullet.back().SetRotation(XMFLOAT3(XMConvertToDegrees(rotx), XMConvertToDegrees(roty),0));
	}

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

	

	// 撃つ
	if (nowShotFrame-- == 0) {
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

}

void Enemy::leaveChange(XMFLOAT3 vel) {
	SetVel(vel);

	// leaveに変える
	phase = std::bind(&Enemy::leave, this);


}