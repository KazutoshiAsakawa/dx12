#pragma once

#include "GameObject.h"
#include "FbxObject3d.h"
#include "FbxModel.h"
#include "PlayerBullet.h"

#include <vector>
#include <memory>

class Player : public GameObject
{
public:
	using GameObject::GameObject;

	Player();

	void Update() override;
	void Draw() override;

	void Shot(ObjModel* model, float scale = 1);

	inline void SetShotTarget(GameObject* shotTarget) { this->shotTarget = shotTarget; }

	inline auto& GetBullet() { return bullet; }

	inline DirectX::XMFLOAT2 GetScreenAimPos() { return float2ScreenAimPos; }

	inline void Damage(UINT damage) { if (hp >= damage)hp -= damage; }
	inline UINT GetHp() { return hp; }

private:

	std::unique_ptr <ObjModel> model;
	std::vector<PlayerBullet> bullet;

	// 照準
	std::unique_ptr <ObjObject3d> aim;

	// 照準のスクリーン座標
	DirectX::XMFLOAT2 float2ScreenAimPos;

	// 標的
	GameObject* shotTarget;

	// 体力
	UINT hp;
};

