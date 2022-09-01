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

	void Move(float speed = 0.2f);

	void Update() override;
	void Draw() override;

	void Shot(ObjModel* model, float scale = 1);

	inline void SetShotTarget(GameObject* shotTarget) {this->shotTarget = shotTarget;}

	inline auto& GetBullet() { return bullet; }

	inline DirectX::XMFLOAT2 GetScreenAimPos() { return float2ScreenAimPos; }

	// void Bullet();

private:

	std::unique_ptr <ObjModel> model;

	std::vector<PlayerBullet> bullet;

	// è∆èÄ
	std::unique_ptr <ObjObject3d> aim;

	DirectX::XMFLOAT2 float2ScreenAimPos;

	// ïWìI
	GameObject* shotTarget;
};

