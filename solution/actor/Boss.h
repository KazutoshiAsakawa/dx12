#pragma once
#include "Enemy.h"
class Boss :
	public Enemy
{

	GameObject* attackTarget;

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

	inline void SetAttackTarget(GameObject* attackTarget) { this->attackTarget = attackTarget; }

	inline void SetPhaseApproach() { SetPhase(std::bind(&Boss::Approach, this)); }

	void Approach();
};

