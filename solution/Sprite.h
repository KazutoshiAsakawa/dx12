#pragma once

#include"SpriteCommon.h"

#include <wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>

/// <summary>
/// スプライト1枚分を表すクラス
/// </summary>
class Sprite
{
public:
	// 頂点データ
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos; // xyz座標
		DirectX::XMFLOAT2 uv;  // uv座標
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		DirectX::XMFLOAT4 color; // 色 (RGBA)
		DirectX::XMMATRIX mat;   // ３Ｄ変換行列
	};
	/// <summary>
	/// スプライト生成
	/// </summary>
	/// <param name="texNumber">テクスチャ番号</param>
	/// <param name="anchorpoint">アンカーポイント</param>
	/// <param name="isFlipX">X反転するか</param>
	/// <param name="isFlipY">Y反転するか</param>
	/// <returns>スプライト</returns>
	static Sprite* Create(UINT texNumber, DirectX::XMFLOAT2 anchorpoint = { 0.5f,0.5f }, bool isFlipX = false, bool isFlipY = false);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(UINT texNumber, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY);

	/// <summary>
	/// 頂点バッファの転送
	/// </summary>
	void TransferVertexBuffer();

	/// <summary>
	/// 枚フレーム更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetSize(const DirectX::XMFLOAT2& size) { this->size = size; }
	void SetTexLeftTop(const DirectX::XMFLOAT2& texLeftTop){ this->texLeftTop = texLeftTop;}
	void SetTexSize(const DirectX::XMFLOAT2& texSize){ this->texSize = texSize;}
	void SetColor(const DirectX::XMFLOAT4& color){ this->color = color;}

	inline const DirectX::XMFLOAT2& GetSize() {return size; };

private:
	//頂点バッファ;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//定数バッファ;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	// Z軸回りの回転角
	float rotation = 0.0f;
	// 座標
	DirectX::XMFLOAT3 position = { 0,0,0 };
	// ワールド行列
	DirectX::XMMATRIX matWorld;
	// 色(RGBA)
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };// 1, 1, 1, 1
	// テクスチャ番号
	UINT texNumber = 0;
	// 大きさ
	DirectX::XMFLOAT2 size = { 100, 100 };
	// アンカーポイント
	DirectX::XMFLOAT2 anchorpoint = { 0.5f, 0.5f };
	// 左右反転
	bool isFlipX = false;
	// 上下反転
	bool isFlipY = false;
	// テクスチャ左上座標
	DirectX::XMFLOAT2 texLeftTop = { 0, 0 };
	// テクスチャ切り出しサイズ
	DirectX::XMFLOAT2 texSize = { 100, 100 };
	// 非表示
	bool isInvisible = false;
};

