#include "PlayerBullet.h"
#include "ParticleLoad.h"

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

	ParticleLoad::GetInstance()->SetRenderAdd(1, rand() % 20, obj->GetWorldPos(), {0.f,0.2f,0.f}, {0.f,0.f,0.f},
		1.0f, (float)rand() / RAND_MAX * 0.5f, { 0.7f, 0.7f, 0.3f }, { 1.f,0.f,0.f });

	obj->Update();
}
