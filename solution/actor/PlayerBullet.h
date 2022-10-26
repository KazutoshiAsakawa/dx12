#pragma once
#include "GameObject.h"
class PlayerBullet :
	public GameObject
{
public:
	using GameObject::GameObject;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 速度を設定
	/// </summary>
	/// <param name="vel">速度</param>
	inline void SetVel(const DirectX::XMFLOAT3& vel) { this->vel = vel; }
	/// <summary>
	/// 速度を取得
	/// </summary>
	/// <returns>速度</returns>
	inline const DirectX::XMFLOAT3& GetVel() { return vel; }


	// 弾の消える時間
	UINT life = 60 * 2;

	UINT numFrame = 0;

private:
	// 速度
	DirectX::XMFLOAT3 vel;
};

