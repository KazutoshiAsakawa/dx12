#include "SceneManager.h"
#include<cassert>

SceneManager::~SceneManager()
{
	// �Ō�̃V�[���̏I���Ɖ��
	scene_->Finalize();
	delete scene_;
}

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Update(DirectXCommon* dxcommon)
{
	// �V�[���؂�ւ�������Ȃ�
	if (nextScene_) {
		if (scene_) {
			// ���V�[���̏I��
			scene_->Finalize();
			delete scene_;
		}
		// �V�[���؂�ւ�
		scene_ = nextScene_;
		nextScene_ = nullptr;

		// �V�V�[���̏�����
		scene_->Initialize(dxcommon);
	}
	scene_->Update();
}

void SceneManager::Draw(DirectXCommon* dxcommon)
{
	scene_->Draw(dxcommon);
}

void SceneManager::DrawFrontSprite(DirectXCommon* dxcommon)
{
	scene_->DrawFrontSprite(dxcommon);
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	// ���V�[���𐶐�
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}
