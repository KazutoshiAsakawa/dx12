#pragma once

#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "ObjObject3d.h"
#include "ObjModel.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Audio.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"

#include<memory>

// フレームワーク
class Framework
{
public:
	/// <summary>
	/// 実行
	/// </summary>
	void Run();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	virtual bool IsEndRequest() { return endRequest_; }

protected:
	bool endRequest_ = false;
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;
	DebugText* debugText = nullptr;
	Audio* audio = nullptr;
	Input* input = nullptr;
	SpriteCommon* spriteCommon = nullptr;
	// 所有者(管理者)
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
};

