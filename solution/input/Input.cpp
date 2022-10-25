#include "Input.h"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include "WinApp.h"

Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

void Input::Initialize(WinApp* winApp)
{
	HRESULT result;

	result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard); // �W���`��

	result = devkeyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	result = dinput->CreateDevice(GUID_SysMouse, &devmouse, NULL);

	result = devmouse->SetDataFormat(&c_dfDIMouse2); // �W���`��

	result = devmouse->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	this->winApp = winApp;
}

void Input::Update()
{
	HRESULT result;

	result = devmouse->Acquire();	// �}�E�X����J�n
	mousePre = mouse;
	result = devmouse->GetDeviceState(sizeof(mouse), &mouse);

	// �X�N���[����ł́u���W�����v
	// &�����Ă���̂Ń|�C���^�n��
	// (�\�z)�|�C���^�n���Ȃ̂Œl�����������Ă���
	GetCursorPos(&mousePos);
	// �X�N���[�����W��
	// �N���C�A���g���W(�E�B���h�E���ł̍��W)
	// �Ɂu�ϊ��v
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &mousePos);

	//�O��̃L�[���͂�ۑ�
	memcpy(keyPre, key, sizeof(key));

	//�L�[�{�[�h���̎擾
	result = devkeyboard->Acquire();
	//�S�L�[�̓��͏����擾����
	result = devkeyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
	//�w��L�[�������Ă����true��Ԃ�
	if (key[keyNumber]) {
		return true;
	}
	//�����łȂ����false��Ԃ�
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (!keyPre[keyNumber] && key[keyNumber]) {
		return true;
	}
	return false;
}