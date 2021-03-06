#include "SceneManager.h"
#include<cassert>

SceneManager::~SceneManager()
{
	// 最後のシーンの終了と解放
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
	// シーン切り替えがあるなら
	if (nextScene_) {
		if (scene_) {
			// 旧シーンの終了
			scene_->Finalize();
			delete scene_;
		}
		// シーン切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;

		// 新シーンの初期化
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

	// 次シーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}
