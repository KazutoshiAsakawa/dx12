#include "Boss.h"

void Boss::Update()
{
	if (alive) {
		phase();

		// 座標変換の計算
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
