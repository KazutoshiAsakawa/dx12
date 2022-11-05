#include "Boss.h"

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
