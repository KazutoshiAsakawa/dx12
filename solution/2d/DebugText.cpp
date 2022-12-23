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
	spriteCommon = spriteCommon;

	// �S�ẴX�v���C�g�f�[�^�ɂ���
	for (int i = 0; i < _countof(sprites); i++)
	{
		// �X�v���C�g�𐶐�����
		sprites[i] = Sprite::Create(texnumber, { 0,0 });
	}
}

void DebugText::Finalize()
{
	// �X�v���C�g�ʉ��
	for (auto& sprite : sprites) {
		delete sprite;
	}
}

void DebugText::Print(const std::string& text, float x, float y, float scale, const DirectX::XMFLOAT4& color)
{
	// �S�Ă̕����ɂ���
	for (int i = 0; i < text.size(); i++)
	{
		// �ő啶��������
		if (spriteIndex >= maxCharCount) {
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
		//sprites[spriteIndex]->position = { x + fontWidth * scale * i, y, 0 };
		sprites[spriteIndex]->SetPosition({ x + fontWidth * scale * i, y, 0 });
		sprites[spriteIndex]->SetTexLeftTop({ (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight });
		sprites[spriteIndex]->SetTexSize({ fontWidth, fontHeight });
		sprites[spriteIndex]->SetSize({ fontWidth * scale, fontHeight * scale });
		sprites[spriteIndex]->SetColor(color);
		// ���_�o�b�t�@�]��
		sprites[spriteIndex]->TransferVertexBuffer();
		// �X�V
		sprites[spriteIndex]->Update();

		// �������P�i�߂�
		spriteIndex++;
	}
}

// �܂Ƃ߂ĕ`��
void DebugText::DrawAll()
{
	// �S�Ă̕����̃X�v���C�g�ɂ���
	for (int i = 0; i < spriteIndex; i++)
	{
		// �X�v���C�g�`��
		sprites[i]->Draw();
	}
	spriteIndex = 0;
}
