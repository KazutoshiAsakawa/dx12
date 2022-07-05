#pragma once
#include <d3d12.h>
#include<d3dx12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include"WinApp.h"

class DirectXCommon
{
public://�����o�[�ϐ�
	//������
	void Initialize(WinApp* winApp);

	void PreDraw();

	void PostDraw();

	UINT64 fenceVal = 0;

	//�f�o�C�X�̎擾
	ID3D12Device* GetDev() { return dev.Get(); }

	ID3D12GraphicsCommandList * GetCmdList() { return cmdList.Get(); }

private:

	void InitializeDevice();

	void InitializeCommand();

	void InitializeSwapchain();

	void InitializeRenderTargetview();

	void InitializeDepthBuffer();

	void InitializeFence();

	//�f�o�C�X
	Microsoft::WRL::ComPtr<ID3D12Device> dev;
	//DXGI�t�@�N�g���I
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

	//WindowsAPI
	WinApp* winApp = nullptr;
};