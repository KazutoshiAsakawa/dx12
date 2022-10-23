#include "SceneManager.h"
#include<cassert>

SceneManager::~SceneManager()
{
	// �Ō�̃V�[���̏I���Ɖ��
	scene->Finalize();
	delete scene;
}

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Update(DirectXCommon* dxcommon)
{
	// �V�[���؂�ւ�������Ȃ�
	if (nextScene) {
		if (scene) {
			// ���V�[���̏I��
			scene->Finalize();
			delete scene;
		}
		// �V�[���؂�ւ�
		scene = nextScene;
		nextScene = nullptr;

		// �V�V�[���̏�����
		scene->Initialize(dxcommon);
	}
	scene->Update();
}

void SceneManager::Draw(DirectXCommon* dxcommon)
{
	scene->Draw(dxcommon);
}

void SceneManager::DrawFrontSprite(DirectXCommon* dxcommon)
{
	scene->DrawFrontSprite(dxcommon);
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory);
	assert(nextScene == nullptr);

	// ���V�[���𐶐�
	nextScene = sceneFactory->CreateScene(sceneName);
}
