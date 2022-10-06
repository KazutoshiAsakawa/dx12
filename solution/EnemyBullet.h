#pragma once
#include "GameObject.h"
class EnemyBullet :
    public GameObject
{
public:
	using GameObject::GameObject;

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// ���x��ݒ�
	/// </summary>
	/// <param name="vel">���x</param>
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }
	/// <summary>
	/// ���x���擾
	/// </summary>
	/// <returns>���x</returns>
	inline const DirectX::XMFLOAT3& GetVel() { return vel; }

	// �e�̏����鎞��
	UINT life = 60 * 10;
	// �e�̏o�Ă鎞��
	UINT numFrame = 0;

private:
	// ���x
	DirectX::XMFLOAT3 vel;
};

