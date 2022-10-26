#include "Boss.h"

void Boss::Update()
{
	if (alive) {
		obj->Update();

		Screen();
	}
}

void Boss::Draw()
{
	if (alive) {
		obj->Draw();
	}
}
