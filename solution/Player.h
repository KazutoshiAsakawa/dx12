#pragma once
#include "FbxObject3d.h"
#include "FbxModel.h"
#include <memory>

class Player
{
public:
	// �R���X�g���N�^
	Player();

	// �f�X�g���N�^
	~Player();

	// �X�V
	void Update();
	//  �`��
	void Draw();

	void PlayAnimation();

	void Move(float speed = 1.f);

private:
	std::unique_ptr <FbxModel> model;
	std::unique_ptr <FbxObject3d> object;
};

