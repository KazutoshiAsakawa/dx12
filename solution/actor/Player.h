#pragma once

#include "GameObject.h"
#include "FbxObject3d.h"
#include "FbxModel.h"
#include "PlayerBullet.h"

#include <vector>
#include <memory>

class Player : public GameObject
{
public:
	using GameObject::GameObject;

	Player();

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="model">���f��</param>
	/// <param name="scale">�傫��</param>
	void Shot(ObjModel* model, float scale = 1);

	/// <summary>
	/// �W�I��ݒ�
	/// </summary>
	/// <param name="shotTarget">�W�I</param>
	inline void SetShotTarget(GameObject* shotTarget) { this->shotTarget = shotTarget; }

	inline auto& GetBullet() { return bullet; }

	/// <summary>
	/// �X�N���[�����W���擾
	/// </summary>
	/// <returns>�X�N���[�����W</returns>
	inline DirectX::XMFLOAT2 GetScreenAimPos() { return float2ScreenAimPos; }

	/// <summary>
	/// �_���[�W
	/// </summary>
	/// <param name="damage">�̗͂����炷�l</param>
	inline void Damage(UINT damage) { if (hp >= damage)hp -= damage; else hp = 0; }
	/// <summary>
	/// �̗͂�ݒ�
	/// </summary>
	/// <param name="hp">�̗�</param>
	inline void SetHp(UINT hp) { this->hp = hp; }
	/// <summary>
	/// �̗͂��擾
	/// </summary>
	/// <returns>�̗�</returns>
	inline UINT GetHp() { return hp; }

private:

	std::unique_ptr <ObjModel> model;
	std::vector<PlayerBullet> bullet;

	// ��蕨
	std::unique_ptr <ObjObject3d> momijiObj;

	// �Ə�
	std::unique_ptr <ObjObject3d> aim;

	// �Ə��̃X�N���[�����W
	DirectX::XMFLOAT2 float2ScreenAimPos;

	// �W�I
	GameObject* shotTarget;

	// �̗�
	UINT hp;
};

