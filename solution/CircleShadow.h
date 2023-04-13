#pragma once

#include <DirectXMath.h>

/// <summary>
/// �ۉe
/// </summary>
class CircleShadow {
private: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMVECTOR dir;
		XMFLOAT3 casterPos;
		float distanceCasterLight;
		XMFLOAT3 atten;
		float pad3;
		XMFLOAT2 factorAngleCos;
		unsigned int active;
		float pad4;
	};

public: // �����o�֐�
	/// <summary>
	/// �������Z�b�g
	/// </summary>
	/// <param name="lightdir">����</param>
	inline void SetDir(const XMVECTOR& dir) { this->dir = DirectX::XMVector3Normalize(dir); }

	/// <summary>
	/// ���C�g�������擾
	/// </summary>
	/// <returns>���C�g����</returns>
	inline const XMVECTOR& GetDir() { return dir; }

	/// <summary>
	/// �L���X�^�[���W���Z�b�g
	/// </summary>
	/// <param name="lightpos">�L���X�^�[���W</param>
	inline void SetCasterPos(const XMFLOAT3& casterPos) { this->casterPos = casterPos; }

	/// <summary>
	/// �L���X�^�[���W���擾
	/// </summary>
	/// <returns>�L���X�^�[���W</returns>
	inline const XMFLOAT3& GetCasterPos() { return casterPos; }

	/// <summary>
	/// �L���X�^�[�ƃ��C�g�̋������Z�b�g
	/// </summary>
	/// <param name="lightpos">�L���X�^�[�ƃ��C�g�̋���</param>
	inline void SetDistanceCasterLight(float distanceCasterLight) { this->distanceCasterLight = distanceCasterLight; }

	/// <summary>
	/// �L���X�^�[�ƃ��C�g�̋������擾
	/// </summary>
	/// <returns>�L���X�^�[�ƃ��C�g�̋���</returns>
	inline float GetDistanceCasterLight() { return distanceCasterLight; }

	/// <summary>
	/// ���������W�����Z�b�g
	/// </summary>
	/// <param name="lightatten">���C�g���������W��</param>
	inline void SetAtten(const XMFLOAT3& atten) { this->atten = atten; }

	/// <summary>
	/// ���������W�����擾
	/// </summary>
	/// <returns>���C�g���������W��</returns>
	inline const XMFLOAT3& GetAtten() { return atten; }

	/// <summary>
	/// �����p�x���Z�b�g
	/// </summary>
	/// <param name="lightFactorAngle">x:�����J�n�p�x y:�����I���p�x[degree]</param>
	inline void SetFactorAngle(const XMFLOAT2& factorAngle) {
		this->factorAngleCos.x = cosf(DirectX::XMConvertToRadians(factorAngle.x));
		this->factorAngleCos.y = cosf(DirectX::XMConvertToRadians(factorAngle.y));
	}

	/// <summary>
	/// �����p�x���擾
	/// </summary>
	/// <returns>�����p�x</returns>
	inline const XMFLOAT2& GetFactorAngleCos() { return factorAngleCos; }

	/// <summary>
	/// �L���t���O���Z�b�g
	/// </summary>
	/// <param name="active">�L���t���O</param>
	inline void SetActive(bool active) { this->active = active; }

	/// <summary>
	/// �L���`�F�b�N
	/// </summary>
	/// <returns>�L���t���O</returns>
	inline bool IsActive() { return active; }

private: // �����o�ϐ�
	// �����i�P�ʃx�N�g���j
	XMVECTOR dir = { 1,0,0,0 };
	// �L���X�^�[�ƃ��C�g�̋���
	float distanceCasterLight = 100.0f;
	// �L���X�^�[���W�i���[���h���W�n�j
	XMFLOAT3 casterPos = { 0,0,0 };
	// ���������W��
	XMFLOAT3 atten = { 0.5f, 0.6f, 0.0f };
	// �����p�x
	XMFLOAT2 factorAngleCos = { 0.2f, 0.5f };
	// �L���t���O
	bool active = false;
};