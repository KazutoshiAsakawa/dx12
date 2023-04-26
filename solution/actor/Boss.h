#pragma once
#include "Enemy.h"
#include "ObjModel.h"

class Boss :
	public Enemy {
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

	inline void SetPhaseApproach() { SetPhase(std::bind(&Boss::PhaseApproach, this)); }

	inline void SetBulletModel(ObjModel* bulletModel) { this->bulletModel = bulletModel; }

	// 近づく
	void PhaseApproach() override;
	// 離れる
	void PhaseLeave() override;
	// 弾発射
	void PhaseAttack();
	// 弾拡散
	void PhaseSpreadAttack();
	//  横移動しながら撃つ
	void PhaseLateralAttack();

	// 近接攻撃
	void meleeAttack();
	// 拡散弾
	void spreadBullet(ObjModel* model, float scale, float angle);

private:
	GameObject* attackTarget;

	ObjModel* bulletModel;

	UINT shotInterval = 14;
	UINT nowShotFrame;
	UINT shotNum = 12;
	UINT nowShotNum;
	float rad = 90;
};

