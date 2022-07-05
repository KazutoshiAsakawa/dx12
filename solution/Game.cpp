#include "Game.h"
#include "SceneFactory.h"

void Game::Initialize()
{
	// ���N���X�̏�����
	Framework::Initialize();

#pragma region �V�[������������
	// �V�[���t�@�N�g���������A�}�l�[�W���ɃZ�b�g
	//sceneFactory_ = new SceneFactory();
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());

	// �V�[���}�l�[�W���[�ɍŏ��̃V�[�����Z�b�g
	SceneManager::GetInstance()->ChangeScene("TITLE");

#pragma endregion �V�[������������
}

void Game::Finalize()
{
	// ���N���X�̏I������
	Framework::Finalize();
}

void Game::Update()
{
	// ���N���X�̍X�V����
	Framework::Update();
}

void Game::Draw()
{
	// ���N���X�̍X�V����
	Framework::Draw();
}
