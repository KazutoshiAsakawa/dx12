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

	void LeaveChange(DirectX::XMFLOAT3 vel);
	void DirectionChange(DirectX::XMFLOAT3 vel);

	inline const DirectX::XMFLOAT3& GetVel() { return vel; }
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }

	inline auto& GetBullet() { return bullet; }

	inline void SetShotTarget(GameObject* shotTarget) { this->shotTarget = shotTarget; }

	inline const DirectX::XMFLOAT3& Getframe() { return vel; }

	inline void SetLifeSpan(const UINT& lifeSpan) { this->lifeSpan = lifeSpan; }
	inline const UINT& GetLifeSpan() { return lifeSpan; }

	inline void Damage(UINT damage) { if (hp >= damage)hp -= damage;else hp = 0; }
	inline void SetHp(UINT hp) { this->hp = hp; }
	inline UINT GetHp() { return hp; }

	inline void SetShake(bool shakeFlag) { this->shakeFlag = shakeFlag; memoryPos = obj->GetPosition(); }
	inline bool GetShake() { return shakeFlag; }
	/// <summary>
	/// 振動
	/// </summary>
	void Shake();


	inline void SetHitStop(bool hitStopFlag) { this->hitStopFlag = hitStopFlag; };
	inline bool GetHitStop() { return hitStopFlag; }
	/// <summary>
	/// ヒットストップ
	/// </summary>
	void hitStop();


	inline void SetPhase(std::function<void()> phase) { this->phase = phase; };
protected:
	// 敵の行動パターン
	std::function<void()> phase;
	virtual void PhaseApproach();
	virtual void PhaseLeave();
	void Direction();

	// モデル
	std::unique_ptr<ObjModel> bulletModel;
	float bulletScale;
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

	// 体力
	UINT hp;

	// 振動
	bool shakeFlag = false;
	const float shakeRateDef = 1.f;
	float shakeRate = shakeRateDef;
	DirectX::XMFLOAT3 memoryPos{};

	// ヒットストップ
	bool hitStopFlag = false;
	UINT hitStopFrameDef = 30;
	UINT hitStopFrame = hitStopFrameDef;
	DirectX::XMFLOAT3 memoryVel{};
};

