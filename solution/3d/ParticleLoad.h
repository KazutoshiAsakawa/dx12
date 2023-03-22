#pragma once
#include "ParticleManager.h"
#include <vector>
#include "Camera.h"
class ParticleLoad {

private:
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:// �ÓI�����o�֐�
	static ParticleLoad* GetInstance();

public:
	/// <summary>
	/// ������
	/// </summary>
	/// <returns></returns>
	void Initialize(ID3D12Device* device);
	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �e�N�X�`�����w�肵�ăp�[�e�B�N�����o��
	/// </summary>
	/// <param name="texnum">�e�N�X�`�����w��</param>
	/// <param name="pos">���W</param>
	/// <param name="particleNum">��</param>
	/// <param name="startScale">�傫��</param>
	/// <param name="vel">���x</param>
	/// <param name="start_col">�ŏ��̐F</param>
	/// <param name="end_col">�Ō�̐F</param>
	void SetRenderParticle(int texnum, const XMFLOAT3& pos, UINT particleNum, float startScale, float vel, XMFLOAT3 start_col = { 1,1,1 }, XMFLOAT3 end_col = { 1,1,1 });

	/// <summary>
	/// �e�N�X�`�����w�肵�ăp�[�e�B�N���̒ǉ�
	/// </summary>
	/// <param name="texnum">��������</param>
	/// <param name="life"></param>
	/// <param name="position">�������W</param>
	/// <param name="velocity">���x</param>
	/// <param name="accel">�����x</param>
	/// <param name="start_scale">�J�n���X�P�[��</param>
	/// <param name="end_scale">�I�����X�P�[��</param>
	/// <param name="start_col">�J�n���̐F</param>
	/// <param name="end_col">�I�����̐F</param>
	void SetRenderAdd(int texnum, int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale, XMFLOAT3 start_col, XMFLOAT3 end_col);

	void SetRenderAdd(int texnum, int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale, float start_rot, float end_rot ,XMFLOAT3 start_col, XMFLOAT3 end_col);

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	void SetCamera(Camera* camera);

private:
	std::vector<ParticleManager*> particles;

	// �J����
	Camera* camera = nullptr;

};