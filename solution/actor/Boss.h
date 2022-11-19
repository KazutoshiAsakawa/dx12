#pragma once
#include "Enemy.h"
#include "ObjModel.h"

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

	inline void SetAttackTarget(GameObject* attackTarget) { this->attackTarget = attackTarget; }

	inline void SetPhaseApproach() { SetPhase(std::bind(&Boss::Approach, this)); }

	inline void SetBulletModel(ObjModel* bulletModel) { this->bulletModel = bulletModel; }

	void Approach();
	void Leave();

	void Attack();

private:
	GameObject* attackTarget;

	ObjModel* bulletModel;

	UINT shotInterval = 60;
	UINT nowShotFrame;
	UINT shotNum = 5;
	UINT nowShotNum;
};

