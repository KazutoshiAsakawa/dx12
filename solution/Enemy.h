#pragma once
#include "GameObject.h"
#include "EnemyBullet.h"
#include <functional>

class Enemy :
	public GameObject
{
public:
	using GameObject::GameObject;
	// コンストラクタ
	Enemy(ObjModel* model,
		const DirectX::XMFLOAT3& position = { 0,0,0 });

	// CSV読み込み
	static std::vector<std::vector<std::string>> LoadCsv(const std::string& FilePath);

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 弾発射
	/// </summary>
	void Shot(ObjModel* model, float scale = 1);

	void leaveChange(DirectX::XMFLOAT3 vel);

	inline const DirectX::XMFLOAT3& GetVel() { return vel; }
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }

	inline auto& GetBullet() { return bullet; }

	inline void SetShotTarget(GameObject* shotTarget) { this->shotTarget = shotTarget; }

	inline const DirectX::XMFLOAT3& Getframe() { return vel; }

	inline void SetLifeSpan(UINT lifeSpan) { this->lifeSpan = lifeSpan; }
	inline UINT GetLifeSpan() { return lifeSpan; }

private:
	// 敵の行動パターン
	std::function<void()> phase;
	void approach();
	void leave();

	// モデル
	std::unique_ptr<ObjModel> bulletModel;
	// 敵の移動速度
	DirectX::XMFLOAT3 vel{};
	// 弾
	std::vector<EnemyBullet> bullet;

	UINT shotInterval = 60;
	UINT nowShotFrame;
	// 標的
	GameObject* shotTarget;

	// 経過フレーム数
	UINT frame;

	// 寿命
	UINT lifeSpan;
};

