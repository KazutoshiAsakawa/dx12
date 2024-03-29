#pragma once
#include <Windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
#include <dinput.h>
#include "WinApp.h"

using namespace Microsoft::WRL;

class Input {
public://メンバー関数
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static Input* GetInstance();

	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();
	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="kyeNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);
	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="kyeNumber">キー番号( DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

	inline POINT GetMousePos() { return mousePos; };
	inline void SetMousePos(const POINT& pos) {
		mousePos = pos;
		// クライアント座標(ゲームウィンドウの座標)からスクリーン座標(パソコンの画面)に変換
		ClientToScreen(WinApp::GetInstance()->GetHwnd(), &mousePos);
		// 変換後の座標の位置にマウスカーソルの位置をセット
		SetCursorPos(mousePos.x, mousePos.y);
		mousePos = pos;
	}

	enum MouseButton {
		LEFT = 0,
		RIGHT = 1
	};

	/// <param name="mouseNumber">0で左クリック,1で右クリック</param>
	inline bool PushMouse(_In_ BYTE mouseNumber) {
		return (bool)mouse.rgbButtons[mouseNumber];
	}

	/// <param name="mouseNumber">0で左クリック,1で右クリック</param>
	inline bool TriggerMouse(_In_ BYTE mouseNumber) {
		return PushMouse(mouseNumber) && !(bool)mousePre.rgbButtons[mouseNumber];
	}

private://メンバー変数
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> devkeyboard;
	ComPtr<IDirectInput8> dinput = nullptr;
	//今回の全キーの状態
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE keyPre[256] = {};
	// マウス座標
	POINT mousePos;

	DIMOUSESTATE2 mouse{};
	DIMOUSESTATE2 mousePre{};
	ComPtr<IDirectInputDevice8> devmouse;

private:
	// WindowsAPI
	WinApp* winApp = nullptr;
};