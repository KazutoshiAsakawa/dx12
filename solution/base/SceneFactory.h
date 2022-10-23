#pragma once
#include "AbstractSceneFactory.h"

// ゲーム用のシーン工場
class SceneFactory :
    public AbstractSceneFactory
{
public:
	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>生成したシーン</returns>
	BaseScene* CreateScene(const std::string& sceneName) override;
};

