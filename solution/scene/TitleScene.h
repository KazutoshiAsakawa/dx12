#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "ObjObject3d.h"
#include "DirectXCommon.h"

#include <unordered_map>

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public BaseScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxcommon) override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;
	void DrawFrontSprite(DirectXCommon* dxcommon) override;

private:
	// フレーム
	UINT frame;

	// タイトル画面
	std::unordered_map<std::string, std::unique_ptr<Sprite>> title;

};

