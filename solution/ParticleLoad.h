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

	void SetRenderParticle(int texnum, const XMFLOAT3& pos, UINT particleNum, float startScale, float vel, XMFLOAT3 start_col = { 1,1,1 }, XMFLOAT3 end_col = { 1,1,1 });

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