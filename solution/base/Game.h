#pragma once

#include "Framework.h"

#include <vector>

/// <summary>
/// ゲーム固有のクラス
/// </summary>
class Game : public Framework {
private:
	// privateなコンストラクタ（シングルトンパターン）
	Game();
	// privateなデストラクタ（シングルトンパターン）
	~Game();
	// コピーコンストラクタを禁止（シングルトンパターン）
	Game(const Game& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const Game& obj) = delete;

public:
	static Game* GetInstance();

	inline void SetEndRequest(bool endRequest) { this->endRequest = endRequest; }

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:
};

