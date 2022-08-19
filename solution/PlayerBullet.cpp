#include "PlayerBullet.h"

void PlayerBullet::Update()
{
	if (++numFrame > life) {
		alive = false;
	}

	auto pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;
	obj->SetPosition(pos);

	obj->Update();
}
