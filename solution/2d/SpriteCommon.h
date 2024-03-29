#pragma once
#include"PipelineSet.h"
#include<DirectXMath.h>
#include <vector>

/// <summary>
/// スプライト共通部分
/// </summary>
class SpriteCommon {
public:
	static SpriteCommon* GetInstance();

	// テクスチャの最大枚数
	static const int kSpriteSRVCount = 512;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	/// <param name="commanddList">コマンドリスト</param>
	/// <param name="window_width">ウィンドウ横幅</param>
	/// <param name="window_height">ウィンドウ縦幅</param>
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commanddList, int window_width, int window_height);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <returns>番号</returns>
	UINT LoadTexture(const wchar_t* filename);

	/// <summary>
	/// ルートディスクリプタテーブルの設定
	/// </summary>
	/// <param name="rootParameterIndex">ルートパラメータ番号</param>
	/// <param name="texnumber">テクスチャ番号</param>
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, UINT texnumber);

	/// <summary>
	/// テクスチャ取得関数
	/// </summary>
	/// <param name="texNumber">テクスチャ番号</param>
	/// <returns>テクスチャリソース</returns>
	ID3D12Resource* GetTexBuff(UINT texNumber);

	const DirectX::XMMATRIX& GetMatProjection() const { return matProjection; }
	ID3D12Device* GetDevice() const { return device; }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList; }



private:
	// パイプラインセット
	PipelineSet pipelineSet;
	// 射影行列
	DirectX::XMMATRIX matProjection{};
	// テクスチャ用デスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;
	// テクスチャリソース（テクスチャバッファ）の配列
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> texBuff;
	// デバイス(借りてくる)
	ID3D12Device* device = nullptr;
	// コマンドリスト(借りてくる)
	ID3D12GraphicsCommandList* commandList = nullptr;
private:
	// パイプライン生成
	void CreateGraphicsPipeline();
};

