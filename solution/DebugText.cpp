#include "DebugText.h"

DebugText* DebugText::GetInstance()
{
	static DebugText instance;

	return &instance;
}

void DebugText::Initialize(SpriteCommon* spriteCommon, UINT texnumber)
{
	// nullptr�`�F�b�N
	assert(spriteCommon);
	// �����������o�ϐ��Ɋi�[
	spriteCommon_ = spriteCommon;

	// �S�ẴX�v���C�g�f�[�^�ɂ���
	for (int i = 0; i < _countof(sprites_); i++)
	{
		// �X�v���C�g�𐶐�����
		sprites_[i] = Sprite::Create(texnumber, { 0,0 });
	}
}

void DebugText::Finalize()
{
	// �X�v���C�g�ʉ��
	for (auto& sprite : sprites_) {
		delete sprite;
	}
}

void DebugText::Print(const std::string& text, float x, float y, float scale)
{
	// �S�Ă̕����ɂ���
	for (int i = 0; i < text.size(); i++)
	{
		// �ő啶��������
		if (spriteIndex_ >= maxCharCount) {
			break;
		}

		// 1�������o��(��ASCII�R�[�h�ł������藧���Ȃ�)
		const unsigned char& character = text[i];

		// ASCII�R�[�h��2�i����΂����ԍ����v�Z
		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// ���W�v�Z
		//sprites[spriteIndex]->position_ = { x + fontWidth * scale * i, y, 0 };
		sprites_[spriteIndex_]->SetPosition({ x + fontWidth * scale * i, y, 0 });
		sprites_[spriteIndex_]->SetTexLeftTop({ (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight });
		sprites_[spriteIndex_]->SetTexSize({ fontWidth, fontHeight });
		sprites_[spriteIndex_]->SetSize({ fontWidth * scale, fontHeight * scale });
		// ���_�o�b�t�@�]��
		sprites_[spriteIndex_]->TransferVertexBuffer();
		// �X�V
		sprites_[spriteIndex_]->Update();

		// �������P�i�߂�
		spriteIndex_++;
	}
}

// �܂Ƃ߂ĕ`��
void DebugText::DrawAll()
{
	// �S�Ă̕����̃X�v���C�g�ɂ���
	for (int i = 0; i < spriteIndex_; i++)
	{
		// �X�v���C�g�`��
		sprites_[i]->Draw();
	}
	spriteIndex_ = 0;
}
