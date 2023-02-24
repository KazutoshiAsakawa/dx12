#include "ParticleLoad.h"

ParticleLoad* ParticleLoad::GetInstance() {
	static ParticleLoad instance;
	return &instance;
}

void ParticleLoad::Initialize(ID3D12Device* device) {

	// ���̃��\�[�X����ڂɓ����
	ParticleManager* particle = new ParticleManager();
	particle->SetFileName(L"Resources/blood.png");
	particle->Initialize(device);
	particles.push_back(particle);

	// �G�t�F�N�g1���ڂɓ����
	ParticleManager* bulletParticle = new ParticleManager();
	bulletParticle->SetFileName(L"Resources/effect1.png");
	bulletParticle->Initialize(device);
	particles.push_back(bulletParticle);

}

void ParticleLoad::Update() {

	for (int i = 0; particles.size() > i; i++) {
		particles[i]->Update();
	}

}

void ParticleLoad::Draw(ID3D12GraphicsCommandList* cmdList) {
	for (int i = 0; particles.size() > i; i++) {
		particles[i]->Draw(cmdList);
	}
}

void ParticleLoad::SetRenderParticle(int texnum, const XMFLOAT3& pos, UINT particleNum, float startScale, float vel, XMFLOAT3 start_col, XMFLOAT3 end_col) {

	// �w��̃��\�[�X��ǂݍ���Ńp�[�e�B�N�����o��
	particles[texnum]->CreateParticle(pos, particleNum, startScale, vel, start_col, end_col);

}

void ParticleLoad::SetRenderAdd(int texnum, int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale, XMFLOAT3 start_col, XMFLOAT3 end_col) {

	particles[texnum]->Add(life, position, velocity, accel, start_scale, end_scale, start_col, end_col);
}

void ParticleLoad::SetCamera(Camera* camera) {
	this->camera = camera;

	for (int i = 0; particles.size() > i; i++) {
		particles[i]->SetCamera(camera);
	}
}

