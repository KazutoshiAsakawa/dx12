#pragma once
#include "Enemy.h"
class Boss :
	public Enemy
{

public:
	using Enemy::Enemy;

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;
};

