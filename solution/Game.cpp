#include "Game.h"
#include "SceneFactory.h"

void Game::Initialize()
{
	// 基底クラスの初期化
	Framework::Initialize();

#pragma region シーン初期化処理
	// シーンファクトリ生成し、マネージャにセット
	//sceneFactory_ = new SceneFactory();
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());

	// シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TITLE");

#pragma endregion シーン初期化処理
}

void Game::Finalize()
{
	// 基底クラスの終了処理
	Framework::Finalize();
}

void Game::Update()
{
	// 基底クラスの更新処理
	Framework::Update();
}

void Game::Draw()
{
	// 基底クラスの更新処理
	Framework::Draw();
}
