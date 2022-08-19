#include "PlayerBullet.h"

void PlayerBullet::Update()
{
	if (++numFrame > life) {
		alive = false;
	}

	auto pos = obj->GetPosition();
	pos.z += 1.f;
	obj->SetPosition(pos);

	obj->Update();
}
