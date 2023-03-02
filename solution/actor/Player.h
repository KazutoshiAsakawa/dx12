#pragma once

#include "GameObject.h"
#include "FbxObject3d.h"
#include "FbxModel.h"
#include "PlayerBullet.h"

#include <vector>
#include <memory>

class Player : public GameObject
{
public:
	using GameObject::GameObject;

	Player();

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 撃つ
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="scale">大きさ</param>
	void Shot(ObjModel* model, float scale = 1);

	/// <summary>
	/// 標的を設定
	/// </summary>
	/// <param name="shotTarget">標的</param>
	inline void SetShotTarget(GameObject* shotTarget) { this->shotTarget = shotTarget; }

	inline auto& GetBullet() { return bullet; }

	/// <summary>
	/// スクリーン座標を取得
	/// </summary>
	/// <returns>スクリーン座標</returns>
	inline DirectX::XMFLOAT2 GetScreenAimPos() { return float2ScreenAimPos; }

	/// <summary>
	/// ダメージ
	/// </summary>
	/// <param name="damage">体力を減らす値</param>
	inline void Damage(UINT damage) { if (hp >= damage)hp -= damage; else hp = 0; }
	/// <summary>
	/// 体力を設定
	/// </summary>
	/// <param name="hp">体力</param>
	inline void SetHp(UINT hp) { this->hp = hp; }
	/// <summary>
	/// 体力を取得
	/// </summary>
	/// <returns>体力</returns>
	inline UINT GetHp() { return hp; }

private:

	std::unique_ptr <ObjModel> model;
	std::vector<PlayerBullet> bullet;

	// 乗り物
	std::unique_ptr <ObjObject3d> momijiObj;

	// 照準
	std::unique_ptr <ObjObject3d> aim;

	// 照準のスクリーン座標
	DirectX::XMFLOAT2 float2ScreenAimPos;

	// 標的
	GameObject* shotTarget;

	// 体力
	UINT hp;
};

