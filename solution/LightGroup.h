#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "DirectionalLight.h"

#include "PointLight.h"

/// <summary>
/// ライト
/// </summary>
class LightGroup {
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // 定数
	static const int DirLightNum = 3;
	// 点光源の数
	static const int PointLightNum = 3;

public: // サブクラス

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		// 環境光の色
		XMFLOAT3 ambientColor;
		float pad1;
		// 平行光源用
		DirectionalLight::ConstBufferData dirLights[DirLightNum];
		// 点光源用
		PointLight::ConstBufferData pointLights[PointLightNum];
	};

public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static LightGroup* Create();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	/// <summary>
	/// 定数バッファ転送
	/// </summary>
	void TransferConstBuffer();

	/// <summary>
	/// 標準のライト設定
	/// </summary>
	void DefaultLightSetting();

	/// <summary>
	/// 環境光のライト色をセット
	/// </summary>
	/// <param name="color">ライト色</param>
	void SetAmbientColor(const XMFLOAT3& color);

	/// <summary>
	/// 平行光源の有効フラグをセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="active">有効フラグ</param>
	void SetDirLightActive(int index, bool active);

	/// <summary>
	/// 平行光源のライト方向をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightdir">ライト方向</param>
	void SetDirLightDir(int index, const XMVECTOR& lightdir);

	/// <summary>
	/// 平行光源のライト色をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightcolor">ライト色</param>
	void SetDirLightColor(int index, const XMFLOAT3& lightcolor);

	void SetPointLightActive(int index, bool active);
	void SetPointLightPos(int index, const XMFLOAT3& lightpos);
	void SetPointLightColor(int index, const XMFLOAT3& lightcolor);
	void SetPointLightAtten(int index, const XMFLOAT3& lightAtten);

private: // メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;

	// 環境光の色
	XMFLOAT3 ambientColor = { 1,1,1 };

	// 平行光源の配列
	DirectionalLight dirLights[DirLightNum];

	// ダーティフラグ
	bool dirty = false;

	// 点光源の配列
	PointLight pointLights[PointLightNum];
};

