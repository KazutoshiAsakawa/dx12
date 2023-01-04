#pragma once

#include "Framework.h"

#include <vector>

/// <summary>
/// �Q�[���ŗL�̃N���X
/// </summary>
class Game : public Framework {
private:
	// private�ȃR���X�g���N�^�i�V���O���g���p�^�[���j
	Game();
	// private�ȃf�X�g���N�^�i�V���O���g���p�^�[���j
	~Game();
	// �R�s�[�R���X�g���N�^���֎~�i�V���O���g���p�^�[���j
	Game(const Game& obj) = delete;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	void operator=(const Game& obj) = delete;

public:
	static Game* GetInstance();

	inline void SetEndRequest(bool endRequest) { this->endRequest = endRequest; }

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

