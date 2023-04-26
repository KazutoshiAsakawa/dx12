#pragma once
#include "Enemy.h"
#include "ObjModel.h"

class Boss :
	public Enemy {
public:
	using Enemy::Enemy;

	/// <summary>
	/// –ˆƒtƒŒ[ƒ€XV
	/// </summary>
	void Update() override;

	/// <summary>
	/// •`‰æ
	/// </summary>
	void Draw() override;

	inline void SetAttackTarget(GameObject* attackTarget) { this->attackTarget = attackTarget; }

	inline void SetPhaseApproach() { SetPhase(std::bind(&Boss::PhaseApproach, this)); }

	inline void SetBulletModel(ObjModel* bulletModel) { this->bulletModel = bulletModel; }

	// ‹ß‚Ã‚­
	void PhaseApproach() override;
	// —£‚ê‚é
	void PhaseLeave() override;
	// ’e”­Ë
	void PhaseAttack();
	// ’eŠgU
	void PhaseSpreadAttack();
	//  ‰¡ˆÚ“®‚µ‚È‚ª‚çŒ‚‚Â
	void PhaseLateralAttack();

	// ‹ßÚUŒ‚
	void meleeAttack();
	// ŠgU’e
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

