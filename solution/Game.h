#pragma once

#include "Framework.h"

#include <vector>

/// <summary>
/// �Q�[���ŗL�̃N���X
/// </summary>
class Game : public Framework
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// �I��
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

private:
};

