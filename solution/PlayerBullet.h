#pragma once
#include "GameObject.h"
class PlayerBullet :
	public GameObject
{
public:
	using GameObject::GameObject;

	void Update() override;

	UINT life = 60;

	UINT numFrame = 0;

private:

};

