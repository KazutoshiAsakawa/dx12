#pragma once
#include "GameObject.h"
#include "EnemyBullet.h"
#include <functional>

class Enemy :
	public GameObject
{
public:
	using GameObject::GameObject;
	// �R���X�g���N�^
	Enemy(ObjModel* model,
		const DirectX::XMFLOAT3& position = { 0,0,0 });

	// CSV�ǂݍ���
	static std::vector<std::vector<std::string>> LoadCsv(const std::string& FilePath);

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �e����
	/// </summary>
	void Shot(ObjModel* model, float scale = 1);

	void LeaveChange(DirectX::XMFLOAT3 vel);
	void DirectionChange(DirectX::XMFLOAT3 vel);

	inline const DirectX::XMFLOAT3& GetVel() { return vel; }
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }

	inline auto& GetBullet() { return bullet; }

	inline void SetShotTarget(GameObject* shotTarget) { this->shotTarget = shotTarget; }

	inline const DirectX::XMFLOAT3& Getframe() { return vel; }

	inline void SetLifeSpan(const UINT& lifeSpan) { this->lifeSpan = lifeSpan; }
	inline const UINT& GetLifeSpan() { return lifeSpan; }

	inline void Damage(UINT damage) { if (hp >= damage)hp -= damage;else hp = 0; }
	inline void SetHp(UINT hp) { this->hp = hp; }
	inline UINT GetHp() { return hp; }

	inline void SetShake(bool shakeFlag) { this->shakeFlag = shakeFlag; memoryPos = obj->GetPosition(); }
	inline bool GetShake() { return shakeFlag; }
	/// <summary>
	/// �U��
	/// </summary>
	void Shake();


	inline void SetHitStop(bool hitStopFlag) { this->hitStopFlag = hitStopFlag; };
	inline bool GetHitStop() { return hitStopFlag; }
	/// <summary>
	/// �q�b�g�X�g�b�v
	/// </summary>
	void hitStop();


	inline void SetPhase(std::function<void()> phase) { this->phase = phase; };
protected:
	// �G�̍s���p�^�[��
	std::function<void()> phase;
	virtual void PhaseApproach();
	virtual void PhaseLeave();
	void Direction();

	// ���f��
	std::unique_ptr<ObjModel> bulletModel;
	float bulletScale;
	// �G�̈ړ����x
	DirectX::XMFLOAT3 vel{};
	// �e
	std::vector<EnemyBullet> bullet;

	UINT shotInterval = 60;
	UINT nowShotFrame;
	// �W�I
	GameObject* shotTarget;

	// �o�߃t���[����
	UINT frame;

	// ����
	UINT lifeSpan;

	// �̗�
	UINT hp;

	// �U��
	bool shakeFlag = false;
	const float shakeRateDef = 1.f;
	float shakeRate = shakeRateDef;
	DirectX::XMFLOAT3 memoryPos{};

	// �q�b�g�X�g�b�v
	bool hitStopFlag = false;
	UINT hitStopFrameDef = 30;
	UINT hitStopFrame = hitStopFrameDef;
	DirectX::XMFLOAT3 memoryVel{};
};

