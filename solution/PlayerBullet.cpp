#include "PlayerBullet.h"

void PlayerBullet::Update()
{
	auto pos = obj->GetPosition();
	pos.z += 1.f;
	obj->SetPosition(pos);

	obj->Update();
}
