#include <windows.h>
#pragma once

// WindowsAPI
class WinApp
{
public: //�ÓI�����o�[�ϐ�
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: //�����o�[�ϐ�
	//������
	void Initialize();
	//�X�V
	void Updata();
	//getter
	HWND GetHwnd() { return hwnd; }
	//getter
	HINSTANCE GetHInstance() { return w.hInstance; }
	//�I��
	void Finalize();
	//���Z�[�W�̏���
	bool ProcessMessage();

public: //�萔
	// �E�B���h�E����
	static const int window_width = 1280;
	// �E�B���h�E�c��
	static const int window_height = 720;

private:
	//�E�B���h�E�n���h��
	HWND hwnd = nullptr;
	//�E�B���h�E�N���X�̐ݒ�
	WNDCLASSEX w{};
};