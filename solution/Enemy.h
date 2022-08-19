#pragma once
#include "GameObject.h"
class Enemy :
	public GameObject
{
public:
	using GameObject::GameObject;

	void Update() override;

	inline const DirectX::XMFLOAT3& GetVel() { return vel; }
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }

private:

	DirectX::XMFLOAT3 vel{};
};

