#include <windows.h>
#pragma once

// WindowsAPI
class WinApp
{
	// private�ȃR���X�g���N�^�i�V���O���g���p�^�[���j
	WinApp();
	// private�ȃf�X�g���N�^�i�V���O���g���p�^�[���j
	~WinApp();
	// �R�s�[�R���X�g���N�^���֎~�i�V���O���g���p�^�[���j
	WinApp(const WinApp& obj) = delete;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	void operator=(const WinApp& obj) = delete;

public: //�ÓI�����o�[�ϐ�
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static WinApp* GetInstance();

private: //�����o�[�ϐ�
	//������
	void Initialize();
	//�I��
	void Finalize();

public:
	//�X�V
	void Updata();
	//getter
	HWND GetHwnd() { return hwnd; }
	//getter
	HINSTANCE GetHInstance() { return w.hInstance; }
	//���Z�[�W�̏���
	bool ProcessMessage();

public: //�萔
	// �E�B���h�E����
	static const int window_width = 1280; // 1280

	// �E�B���h�E�c��
	static const int window_height = 720; // 720

private:
	//�E�B���h�E�n���h��
	HWND hwnd = nullptr;
	//�E�B���h�E�N���X�̐ݒ�
	WNDCLASSEX w{};
};