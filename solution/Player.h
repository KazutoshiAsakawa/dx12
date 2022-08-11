#pragma once
#include "FbxObject3d.h"
#include "FbxModel.h"
#include <memory>

class Player
{
public:
	// コンストラクタ
	Player();

	// デストラクタ
	~Player();

	// 更新
	void Update();
	//  描画
	void Draw();

	void PlayAnimation();

	void Move(float speed = 1.f);

private:
	std::unique_ptr <FbxModel> model;
	std::unique_ptr <FbxObject3d> object;
};

