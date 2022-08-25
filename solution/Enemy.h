#pragma once
#include "GameObject.h"
#include "EnemyBullet.h"
#include <functional>

class Enemy :
	public GameObject
{
public:
	using GameObject::GameObject;

	Enemy(ObjModel* model,
		const DirectX::XMFLOAT3& position = { 0,0,0 });

	void Update() override;
	void Draw() override;

	void Shot(ObjModel* model, float scale = 1);

	inline const DirectX::XMFLOAT3& GetVel() { return vel; }
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }

	inline auto& GetBullet() { return bullet; }

private:

	std::function<void()> phase;
	void approach();
	void leave();

	std::unique_ptr<ObjModel> bulletModel;

	// “G‚ÌˆÚ“®‘¬“x
	DirectX::XMFLOAT3 vel{};

	std::vector<EnemyBullet> bullet;
	UINT shotInterval = 60;
	UINT nowShotFrame;
};

