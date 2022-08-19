#include "Enemy.h"
using namespace DirectX;

void Enemy::Update()
{
	if (alive) {

		XMFLOAT3 pos = obj->GetPosition();
		pos.x += vel.x;
		pos.y += vel.y;
		pos.z += vel.z;

		obj->SetPosition(pos);
		obj->Update();
	}
}
