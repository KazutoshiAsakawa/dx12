#pragma once
#include "Enemy.h"
#include "ObjModel.h"

class Boss :
	public Enemy {
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

	inline void SetPhaseApproach() { SetPhase(std::bind(&Boss::PhaseApproach, this)); }

	inline void SetBulletModel(ObjModel* bulletModel) { this->bulletModel = bulletModel; }

	// �߂Â�
	void PhaseApproach() override;
	// �����
	void PhaseLeave() override;
	// �e����
	void PhaseAttack();
	// �e�g�U
	void PhaseSpreadAttack();
	//  ���ړ����Ȃ��猂��
	void PhaseLateralAttack();

	// �ߐڍU��
	void meleeAttack();
	// �g�U�e
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

