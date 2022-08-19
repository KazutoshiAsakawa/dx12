#pragma once

#include "GameObject.h"
#include "FbxObject3d.h"
#include "FbxModel.h"
#include <memory>

class Player : public GameObject
{
public:
	using GameObject::GameObject;

	Player();

	void Move(float speed = 0.2f);

	void Update() override;

	// void Bullet();

private:
	std::unique_ptr <ObjModel> model;
};

