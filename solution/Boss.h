#pragma once
#include "Enemy.h"
class Boss :
	public Enemy
{

public:
	using Enemy::Enemy;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
};

