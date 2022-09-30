#include "Boss.h"

void Boss::Update()
{
	if (alive) {
		obj->Update();
	}
}

void Boss::Draw()
{
	if (alive) {
		obj->Draw();
	}
}
