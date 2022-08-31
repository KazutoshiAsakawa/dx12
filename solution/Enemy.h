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

	void leaveChange(DirectX::XMFLOAT3 vel);

	inline const DirectX::XMFLOAT3& GetVel() { return vel; }
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }

	inline auto& GetBullet() { return bullet; }

	inline void SetShotTarget(GameObject* shotTarget) { this->shotTarget = shotTarget; }

	inline const DirectX::XMFLOAT3& Getframe() { return vel; }

	inline void SetLifeSpan(UINT lifeSpan) { this->lifeSpan = lifeSpan; }
	inline UINT GetLifeSpan() { return lifeSpan; }

private:
	// �G�̍s���p�^�[��
	std::function<void()> phase;
	void approach();
	void leave();

	// ���f��
	std::unique_ptr<ObjModel> bulletModel;
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
};

