#pragma once

#include "Sprite.h"

#include <windows.h>
#include <string>

/// <summary>
/// �f�o�b�O������N���X�̒�`
/// </summary>
class DebugText
{
public: // �萔�̐錾    
	static const int maxCharCount = 256;    // �ő啶����
	static const int fontWidth = 9;         // �t�H���g�摜��1�������̉���
	static const int fontHeight = 18;       // �t�H���g�摜��1�������̏c��
	static const int fontLineCount = 14;    // �t�H���g�摜��1�s���̕�����

public: // �����o�֐�
	static DebugText* GetInstance();

	void Initialize(SpriteCommon* spriteCommon, UINT texnumber);

	void Finalize();

	void Print(const std::string& text, float x, float y, float scale = 1.0f);

	void DrawAll();

private: // �����o�ϐ�
	// �X�v���C�g���ʕ���(�؂�Ă���)
	SpriteCommon* spriteCommon_ = nullptr;
	// �X�v���C�g�f�[�^�̔z��
	Sprite* sprites_[maxCharCount];
	// �X�v���C�g�f�[�^�z��̓Y�����ԍ�
	int spriteIndex_ = 0;
};
