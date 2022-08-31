#pragma once
#include "GameObject.h"
class EnemyBullet :
    public GameObject
{
public:
	using GameObject::GameObject;

	void Update() override;

	inline const DirectX::XMFLOAT3& GetVel() { return vel; }
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }

	// ’e‚ÌÁ‚¦‚éŠÔ
	UINT life = 60 * 50;

	UINT numFrame = 0;

private:
	DirectX::XMFLOAT3 vel;
};

