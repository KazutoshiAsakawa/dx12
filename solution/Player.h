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

	inline auto& GetBullet() { return bullet; }

	// void Bullet();

private:

	std::unique_ptr <ObjModel> model;

	std::vector<PlayerBullet> bullet;
};

