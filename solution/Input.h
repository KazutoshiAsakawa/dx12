#pragma once
#include <Windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION     0x0800   // DirectInput�̃o�[�W�����w��
#include <dinput.h>
#include "WinApp.h"

using namespace Microsoft::WRL;

class Input
{
public://�����o�[�֐�
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static Input* GetInstance();

	//������
	void Initialize(WinApp* winApp);
	//�X�V
	void Update();
	/// <summary>
	/// �L�[�̉������`�F�b�N
	/// </summary>
	/// <param name="kyeNumber">�L�[�ԍ�( DIK_0 ��)</param>
	/// <returns>������Ă��邩</returns>
	bool PushKey(BYTE keyNumber);
	/// <summary>
	/// �L�[�̃g���K�[���`�F�b�N
	/// </summary>
	/// <param name="kyeNumber">�L�[�ԍ�( DIK_0 ��)</param>
	/// <returns>�g���K�[��</returns>
	bool TriggerKey(BYTE keyNumber);

	inline POINT GetMousePos() { return mousePos; };
	inline void SetMousePos(const POINT& pos) {
		mousePos = pos;
		// �N���C�A���g���W(�Q�[���E�B���h�E�̍��W)����X�N���[�����W(�p�\�R���̉��)�ɕϊ�
		ClientToScreen(WinApp::GetInstance()->GetHwnd(), &mousePos);
		// �ϊ���̍��W�̈ʒu�Ƀ}�E�X�J�[�\���̈ʒu���Z�b�g
		SetCursorPos(mousePos.x, mousePos.y);
		mousePos = pos;
	}

	enum MouseButton
	{
		LEFT = 0,
		RIGHT = 1
	};

	/// <param name="mouseNumber">0�ō��N���b�N,1�ŉE�N���b�N</param>
	inline bool PushMouse(_In_ BYTE mouseNumber)
	{
		return (bool)mouse.rgbButtons[mouseNumber];
	}

	/// <param name="mouseNumber">0�ō��N���b�N,1�ŉE�N���b�N</param>
	inline bool TriggerMouse(_In_ BYTE mouseNumber)
	{
		return PushMouse(mouseNumber) && !(bool)mousePre.rgbButtons[mouseNumber];
	}

private://�����o�[�ϐ�
	//�L�[�{�[�h�̃f�o�C�X
	ComPtr<IDirectInputDevice8> devkeyboard;
	ComPtr<IDirectInput8> dinput = nullptr;
	//����̑S�L�[�̏��
	BYTE key[256] = {};
	//�O��̑S�L�[�̏��
	BYTE keyPre[256] = {};

	POINT mousePos;

	DIMOUSESTATE2 mouse{};
	DIMOUSESTATE2 mousePre{};
	ComPtr<IDirectInputDevice8> devmouse;

private:
	// WindowsAPI
	WinApp* winApp = nullptr;
};