#include <windows.h>
#pragma once

// WindowsAPI
class WinApp
{
public: //静的メンバー変数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: //メンバー変数
	//初期化
	void Initialize();
	//更新
	void Updata();
	//getter
	HWND GetHwnd() { return hwnd; }
	//getter
	HINSTANCE GetHInstance() { return w.hInstance; }
	//終了
	void Finalize();
	//メセージの処理
	bool ProcessMessage();

public: //定数
	// ウィンドウ横幅
	static const int window_width = 1280;
	// ウィンドウ縦幅
	static const int window_height = 720;

private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;
	//ウィンドウクラスの設定
	WNDCLASSEX w{};
};