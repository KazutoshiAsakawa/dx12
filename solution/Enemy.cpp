#include "Enemy.h"
using namespace DirectX;

Enemy::Enemy(ObjModel* model, const DirectX::XMFLOAT3& position)
:GameObject(model,position),phase(std::bind(&Enemy::approach,this)) {

}

void Enemy::Update()
{
	if (alive) {

		phase();
	}
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
