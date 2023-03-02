#pragma once

#include "ObjModel.h"
#include "ObjObject3d.h"
#include <DirectXMath.h>
#include <memory>

class GameObject {

protected:
	std::unique_ptr<ObjObject3d> obj;
	bool alive = true;
	bool drawFlag = true;

	DirectX::XMFLOAT2 float2ScreenPos{};

public:
	/// <summary>
	/// 生存フラグの設定
	/// </summary>
	/// <param name="alive">生存フラグ</param>
	inline void SetAlive(const bool& alive) { this->alive = alive; }
	/// <summary>
	/// 生存フラグの取得
	/// </summary>
	/// <returns>生存フラグ</returns>
	inline bool GetAlive() const { return alive; }

	/// <summary>
	/// 描画フラグの設定
	/// </summary>
	/// <param name="alive">描画フラグ</param>
	inline void SetDrawFlag(bool drawFlag) { this->drawFlag = drawFlag; }
	/// <summary>
	/// 描画フラグの取得
	/// </summary>
	/// <returns>描画フラグ</returns>
	inline bool GetDrawFlag() const { return drawFlag; }

	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	inline void SetPosition(const DirectX::XMFLOAT3& position) { obj->SetPosition(position); }
	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	inline const DirectX::XMFLOAT3& GetPosition() const { return obj->GetPosition(); }

	/// <summary>
	/// 大きさの設定
	/// </summary>
	/// <param name="scale">大きさ</param>
	inline void SetScale(const DirectX::XMFLOAT3& scale) { obj->SetScale(scale); }
	/// <summary>
	/// 大きさの取得
	/// </summary>
	/// <returns>大きさ</returns>
	inline const DirectX::XMFLOAT3& GetScale() const { return obj->GetScale(); }

	/// <summary>
	/// 回転の設定
	/// </summary>
	/// <param name="rotation">回転</param>
	inline void SetRotation(const DirectX::XMFLOAT3& rotation) { obj->SetRotation(rotation); }
	/// <summary>
	/// 回転の取得
	/// </summary>
	/// <returns>回転</returns>
	inline const DirectX::XMFLOAT3& GetRotation() const { return obj->GetRotation(); }

	/// <summary>
	/// 回転行列の取得
	/// </summary>
	/// <returns>回転行列</returns>
	inline const DirectX::XMMATRIX& GetMatRotation() const { return obj->GetMatRot(); }

	/// <summary>
	/// ペアレントを設定
	/// </summary>
	/// <param name="parent">ペアレント</param>
	inline void SetParent(ObjObject3d* parent) { obj->SetParent(parent); }

	/// <summary>
	/// ペアレントを取得
	/// </summary>
	/// <returns>ペアレント</returns>
	inline ObjObject3d* GetParent() { return obj->GetParent(); }

	/// <summary>
	/// ビルボードの設定
	/// </summary>
	/// <param name="isBillboard">ビルボードフラグ</param>
	void SetIsBillboard(bool isBillboard) { obj->SetIsBillboard(isBillboard); }

	/// <summary>
	/// Y軸ビルボードの設定
	/// </summary>
	/// <param name="isBillboardY">ビルボードフラグ</param>
	void SetIsBillboardY(bool isBillboardY) { obj->SetIsBillboardY(isBillboardY); }

	/// <summary>
	/// オブジェクトを取得
	/// </summary>
	/// <returns>オブジェクト</returns>
	inline ObjObject3d* GetObj() { return obj.get(); }

	/// <summary>
	/// スクリーン座標を取得
	/// </summary>
	/// <returns>スクリーン座標</returns>
	inline const DirectX::XMFLOAT2& GetFloat2ScreenPos() const { return float2ScreenPos; }


	GameObject(
		ObjModel* model,
		const DirectX::XMFLOAT3& position = { 0,0,0 });

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	// スクリーン座標に変換する
	void Screen();
};