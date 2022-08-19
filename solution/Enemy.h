#pragma once
#include "GameObject.h"
class Enemy :
    public GameObject
{
public:
	using GameObject::GameObject;

	void Update() override;

private:

};

