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

private://�����o�[�ϐ�
	//�L�[�{�[�h�̃f�o�C�X
	ComPtr<IDirectInputDevice8> devkeyboard;
	ComPtr<IDirectInput8> dinput = nullptr;
	//����̑S�L�[�̏��
	BYTE key[256] = {};
	//�O��̑S�L�[�̏��
	BYTE keyPre[256] = {};

	POINT mousePos;

private:
	// WindowsAPI
	WinApp* winApp = nullptr;
};