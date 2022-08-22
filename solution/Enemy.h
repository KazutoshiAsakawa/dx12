#pragma once
#include "GameObject.h"
#include <functional>

class Enemy :
	public GameObject
{
public:
	using GameObject::GameObject;

	Enemy(ObjModel* model,
		const DirectX::XMFLOAT3& position = { 0,0,0 });

	void Update() override;

	inline const DirectX::XMFLOAT3& GetVel() { return vel; }
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }

private:

	std::function<void()> phase;
	void approach();
	void leave();

	// “G‚ÌˆÚ“®‘¬“x
	DirectX::XMFLOAT3 vel{};
};

