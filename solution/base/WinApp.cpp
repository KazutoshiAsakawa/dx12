#include "WinApp.h"
#include <windows.h>
#include <imgui_impl_win32.h>
#include <random>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WinApp* WinApp::GetInstance() {
	static WinApp winApp{};
	return &winApp;
}

WinApp::WinApp() {
	Initialize();
}

WinApp::~WinApp() {
	Finalize();
}

void WinApp::Initialize() {
	// ゲーム名
	constexpr const wchar_t gameName[] = L"狐火";

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // ウィンドウプロシージャを設定
	w.lpszClassName = gameName; // ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr); // ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW); // カーソル指定

	// ウィンドウクラスをOSに登録
	RegisterClassEx(&w);
	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // 自動でサイズ補正

	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(w.lpszClassName,	// クラス名
		gameName,							// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,				// 標準的なウィンドウスタイル
		CW_USEDEFAULT,						// 表示X座標（OSに任せる）
		CW_USEDEFAULT,						// 表示Y座標（OSに任せる）
		wrc.right - wrc.left,				// ウィンドウ横幅
		wrc.bottom - wrc.top,				// ウィンドウ縦幅
		nullptr,							// 親ウィンドウハンドル
		nullptr,							// メニューハンドル
		w.hInstance,						// 呼び出しアプリケーションハンドル
		nullptr);							// オプション

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
}

void WinApp::Updata() {

}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return 1;
	}

	//メセージで分岐
	switch (msg) {
	case WM_DESTROY: //ウィンドウが破棄された
		PostQuitMessage(0); //OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::Finalize() {
	// ウィンドウクラスを登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
}

bool WinApp::ProcessMessage() {
	MSG msg{};

	// メッセージがある？
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg); // キー入力メッセージの処理
		DispatchMessage(&msg); // プロシージャにメッセージを送る
	}

	// ✖ボタンで終了メッセージが来たらゲームループを抜ける
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}

float WinApp::MyRand(float center, float range)

{
	// 乱数生成器
	static std::mt19937_64 mt64(0);

	// 乱数生成器
	std::normal_distribution<> rnd(center, range);

	// 乱数を生成
	return (float)rnd(mt64);
}

float WinApp::MyRandMinMax(float min, float max) {
	// 乱数生成器
	static std::mt19937_64 mt64(0);
	std::uniform_real_distribution<float> rnd(min, max);
	return rnd(mt64);
}

int WinApp::MyRandMinMax(int min, int max) {
	// 乱数生成器
	static std::mt19937_64 mt64(0);
	std::uniform_int_distribution<int> rnd(min, max);
	return rnd(mt64);
}
