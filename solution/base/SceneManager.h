#pragma once

#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager {
public:
	static SceneManager* GetInstance();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(DirectXCommon* dxcommon);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxcommon);

	/// <summary>
	/// 前景スプライト描画
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon);

	/// <summary>
	/// 次シーン予約
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory = sceneFactory; }
private:

	// シーン
	BaseScene* scene = nullptr;
	// 次のシーン
	BaseScene* nextScene = nullptr;
	// シーンファクトリー(借りてくる)
	AbstractSceneFactory* sceneFactory = nullptr;

	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	void operator = (const SceneManager&) = delete;
};

