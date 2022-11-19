#include "Boss.h"

using namespace DirectX;

void Boss::Update()
{
	if (alive) {
		phase();

		// À•W•ÏŠ·‚ÌŒvŽZ
		Screen();
		obj->Update();


	}
}

void Boss::Draw()
{
	if (alive) {
		obj->Draw();
	}
}

void Boss::Approach()
{
	XMFLOAT3 pos;
	pos = this->GetPosition();

	pos.x += 0.1f;
	SetPosition(pos);
}