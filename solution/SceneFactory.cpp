#include "SceneFactory.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "EndScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	BaseScene* newScene  = nullptr;
	
	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	}
	else if (sceneName == "GAMEPLAY") {
		newScene = new GamePlayScene();
	}
	else if (sceneName == "END") {
		newScene = new EndScene();
	}

	return newScene;
}
