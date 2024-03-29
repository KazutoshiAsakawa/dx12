#pragma once
#include <d3d12.h>
#include<d3dx12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include <imgui.h>
#include"WinApp.h"
#include<chrono>
#include<thread>

class DirectXCommon {
	// privateなコンストラクタ（シングルトンパターン）
	DirectXCommon();
	// privateなデストラクタ（シングルトンパターン）
	~DirectXCommon();
	// コピーコンストラクタを禁止（シングルトンパターン）
	DirectXCommon(const DirectXCommon& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const DirectXCommon& obj) = delete;

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	using steady_clock = std::chrono::steady_clock;
	using microseconds = std::chrono::microseconds;
	using time_point = steady_clock::time_point;


public:
	static DirectXCommon* GetInstance();

private:
	//初期化
	void Initialize(WinApp* winApp);


public://メンバー変数

	void PreDraw();

	void PostDraw();

	UINT64 fenceVal = 0;

	//デバイスの取得
	ID3D12Device* GetDev() { return dev.Get(); }

	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }

private:

	void InitializeDevice();

	void InitializeCommand();

	void InitializeSwapchain();

	void InitializeRenderTargetview();

	void InitializeDepthBuffer();

	void InitializeFence();

	bool InitializeImgui();

	void InitializeFixFPS();
	void UpdateFixFPS();

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> dev;
	//DXGIファクトリオ
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> imguiHeap;

	steady_clock::time_point reference_;

	//WindowsAPI
	WinApp* winApp = nullptr;
};