#include "Boss.h"

void Boss::Update()
{
	if (alive) {
		phase();

		// ���W�ϊ��̌v�Z
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
