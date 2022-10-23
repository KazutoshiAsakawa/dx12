#include "SceneManager.h"
#include<cassert>

SceneManager::~SceneManager()
{
	// 最後のシーンの終了と解放
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
	// シーン切り替えがあるなら
	if (nextScene) {
		if (scene) {
			// 旧シーンの終了
			scene->Finalize();
			delete scene;
		}
		// シーン切り替え
		scene = nextScene;
		nextScene = nullptr;

		// 新シーンの初期化
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

	// 次シーンを生成
	nextScene = sceneFactory->CreateScene(sceneName);
}
