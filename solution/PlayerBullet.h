#pragma once
#include "GameObject.h"
class PlayerBullet :
	public GameObject
{
public:
	using GameObject::GameObject;

	void Update() override;

private:

};

