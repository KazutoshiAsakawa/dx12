#pragma once

#include "ObjModel.h"
#include "ObjObject3d.h"
#include <DirectXMath.h>
#include <memory>

class GameObject {

protected:
	std::unique_ptr<ObjObject3d> obj;
	bool alive = true;
	bool drawFlag = true;

	DirectX::XMFLOAT2 float2ScreenPos{};

public:
	/// <summary>
	/// �����t���O�̐ݒ�
	/// </summary>
	/// <param name="alive">�����t���O</param>
	inline void SetAlive(const bool& alive) { this->alive = alive; }
	/// <summary>
	/// �����t���O�̎擾
	/// </summary>
	/// <returns>�����t���O</returns>
	inline bool GetAlive() const { return alive; }

	/// <summary>
	/// �`��t���O�̐ݒ�
	/// </summary>
	/// <param name="alive">�`��t���O</param>
	inline void SetDrawFlag(bool drawFlag) { this->drawFlag = drawFlag; }
	/// <summary>
	/// �`��t���O�̎擾
	/// </summary>
	/// <returns>�`��t���O</returns>
	inline bool GetDrawFlag() const { return drawFlag; }

	/// <summary>
	/// ���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	inline void SetPosition(const DirectX::XMFLOAT3& position) { obj->SetPosition(position); }
	/// <summary>
	/// ���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	inline const DirectX::XMFLOAT3& GetPosition() const { return obj->GetPosition(); }

	/// <summary>
	/// �傫���̐ݒ�
	/// </summary>
	/// <param name="scale">�傫��</param>
	inline void SetScale(const DirectX::XMFLOAT3& scale) { obj->SetScale(scale); }
	/// <summary>
	/// �傫���̎擾
	/// </summary>
	/// <returns>�傫��</returns>
	inline const DirectX::XMFLOAT3& GetScale() const { return obj->GetScale(); }

	/// <summary>
	/// ��]�̐ݒ�
	/// </summary>
	/// <param name="rotation">��]</param>
	inline void SetRotation(const DirectX::XMFLOAT3& rotation) { obj->SetRotation(rotation); }
	/// <summary>
	/// ��]�̎擾
	/// </summary>
	/// <returns>��]</returns>
	inline const DirectX::XMFLOAT3& GetRotation() const { return obj->GetRotation(); }

	/// <summary>
	/// ��]�s��̎擾
	/// </summary>
	/// <returns>��]�s��</returns>
	inline const DirectX::XMMATRIX& GetMatRotation() const { return obj->GetMatRot(); }

	/// <summary>
	/// �y�A�����g��ݒ�
	/// </summary>
	/// <param name="parent">�y�A�����g</param>
	inline void SetParent(ObjObject3d* parent) { obj->SetParent(parent); }

	/// <summary>
	/// �y�A�����g���擾
	/// </summary>
	/// <returns>�y�A�����g</returns>
	inline ObjObject3d* GetParent() { return obj->GetParent(); }

	/// <summary>
	/// �r���{�[�h�̐ݒ�
	/// </summary>
	/// <param name="isBillboard">�r���{�[�h�t���O</param>
	void SetIsBillboard(bool isBillboard) { obj->SetIsBillboard(isBillboard); }

	/// <summary>
	/// Y���r���{�[�h�̐ݒ�
	/// </summary>
	/// <param name="isBillboardY">�r���{�[�h�t���O</param>
	void SetIsBillboardY(bool isBillboardY) { obj->SetIsBillboardY(isBillboardY); }

	/// <summary>
	/// �I�u�W�F�N�g���擾
	/// </summary>
	/// <returns>�I�u�W�F�N�g</returns>
	inline ObjObject3d* GetObj() { return obj.get(); }

	/// <summary>
	/// �X�N���[�����W���擾
	/// </summary>
	/// <returns>�X�N���[�����W</returns>
	inline const DirectX::XMFLOAT2& GetFloat2ScreenPos() const { return float2ScreenPos; }


	GameObject(
		ObjModel* model,
		const DirectX::XMFLOAT3& position = { 0,0,0 });

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw();

	// �X�N���[�����W�ɕϊ�����
	void Screen();
};