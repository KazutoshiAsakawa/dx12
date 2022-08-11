#include <windows.h>
#pragma once

// WindowsAPI
class WinApp
{
	// privateなコンストラクタ（シングルトンパターン）
	WinApp();
	// privateなデストラクタ（シングルトンパターン）
	~WinApp();
	// コピーコンストラクタを禁止（シングルトンパターン）
	WinApp(const WinApp& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const WinApp& obj) = delete;

public: //静的メンバー変数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static WinApp* GetInstance();

private: //メンバー変数
	//初期化
	void Initialize();
	//終了
	void Finalize();

public:
	//更新
	void Updata();
	//getter
	HWND GetHwnd() { return hwnd; }
	//getter
	HINSTANCE GetHInstance() { return w.hInstance; }
	//メセージの処理
	bool ProcessMessage();

public: //定数
	// ウィンドウ横幅
	static const int window_width = 1280; // 1280

	// ウィンドウ縦幅
	static const int window_height = 720; // 720

private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;
	//ウィンドウクラスの設定
	WNDCLASSEX w{};
};