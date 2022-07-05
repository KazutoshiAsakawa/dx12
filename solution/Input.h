#pragma once
#include <Windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
#include <dinput.h>
#include "WinApp.h"

using namespace Microsoft::WRL;

class Input
{
public://メンバー関数
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static Input* GetInstance();

	//初期化
	void Initialize(WinApp * winApp);
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
private://メンバー変数
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> devkeyboard;
	ComPtr<IDirectInput8> dinput = nullptr;
	//今回の全キーの状態
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE keyPre[256] = {};
private:
	// WindowsAPI
	WinApp* winApp = nullptr;
};