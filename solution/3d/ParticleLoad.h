#pragma once
#include "ParticleManager.h"
#include <vector>
#include "Camera.h"
class ParticleLoad {

private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:// 静的メンバ関数
	static ParticleLoad* GetInstance();

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	void Initialize(ID3D12Device* device);
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// テクスチャを指定してパーティクルを出す
	/// </summary>
	/// <param name="texnum">テクスチャを指定</param>
	/// <param name="pos">座標</param>
	/// <param name="particleNum">数</param>
	/// <param name="startScale">大きさ</param>
	/// <param name="vel">速度</param>
	/// <param name="start_col">最初の色</param>
	/// <param name="end_col">最後の色</param>
	void SetRenderParticle(int texnum, const XMFLOAT3& pos, UINT particleNum, float startScale, float vel, XMFLOAT3 start_col = { 1,1,1 }, XMFLOAT3 end_col = { 1,1,1 });

	/// <summary>
	/// テクスチャを指定してパーティクルの追加
	/// </summary>
	/// <param name="texnum">生存時間</param>
	/// <param name="life"></param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="start_scale">開始時スケール</param>
	/// <param name="end_scale">終了時スケール</param>
	/// <param name="start_col">開始時の色</param>
	/// <param name="end_col">終了時の色</param>
	void SetRenderAdd(int texnum, int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale, XMFLOAT3 start_col, XMFLOAT3 end_col);

	void SetRenderAdd(int texnum, int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale, float start_rot, float end_rot ,XMFLOAT3 start_col, XMFLOAT3 end_col);

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

private:
	std::vector<ParticleManager*> particles;

	// カメラ
	Camera* camera = nullptr;

};