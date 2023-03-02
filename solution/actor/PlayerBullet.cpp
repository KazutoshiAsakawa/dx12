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
	
	ParticleLoad::GetInstance()->SetRenderAdd(1, rand() % 20, pos, { 0.f,0.2f,0.f }, { 0.2f,0.2f,0.2f },
		1.0f, (float)rand() / RAND_MAX * 0.5f, { 0.07f, 0.0f, 1.5f }, { 0.f,0.f,0.f });

	obj->SetPosition(pos);
	obj->Update();
}
