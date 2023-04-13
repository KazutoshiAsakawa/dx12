#include "ParticleLoad.h"

ParticleLoad* ParticleLoad::GetInstance() {
	static ParticleLoad instance;
	return &instance;
}

void ParticleLoad::Initialize(ID3D12Device* device) {

	// 血のリソースを一個目に入れる
	auto& particle = particles.emplace_back(std::make_unique<ParticleManager>());
	particle->SetFileName(L"Resources/blood.png");
	particle->Initialize(device);

	// エフェクト1を二個目に入れる
	auto& bulletParticle = particles.emplace_back(std::make_unique<ParticleManager>());
	bulletParticle->SetFileName(L"Resources/effect.png");
	bulletParticle->Initialize(device);

	// もみじを三個目に入れる
	auto& leaves = particles.emplace_back(std::make_unique<ParticleManager>());
	leaves->SetFileName(L"Resources/leaves.png");
	leaves->Initialize(device);
}

void ParticleLoad::Update() {

	for (auto& i : particles) {
		i->Update();
	}
}

void ParticleLoad::Draw(ID3D12GraphicsCommandList* cmdList) {

	for (auto& i : particles) {
		i->Draw(cmdList);
	}
}

void ParticleLoad::SetRenderParticle(int texnum, const XMFLOAT3& pos, UINT particleNum,
	float startScale, float vel, XMFLOAT3 start_col, XMFLOAT3 end_col) {

	// 指定のリソースを読み込んでパーティクルを出す
	particles[texnum]->CreateParticle(pos, particleNum, startScale, vel, start_col, end_col);

}

void ParticleLoad::SetRenderAdd(int texnum, int life, XMFLOAT3 position, XMFLOAT3 velocity,
	XMFLOAT3 accel, float start_scale, float end_scale, XMFLOAT3 start_col, XMFLOAT3 end_col) {

	particles[texnum]->Add(life, position, velocity, accel, start_scale, end_scale, 0.0, 0.0, start_col, end_col);
}

void ParticleLoad::SetRenderAdd(int texnum, int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale, float start_rot, float end_rot, XMFLOAT3 start_col, XMFLOAT3 end_col) {

	particles[texnum]->Add(life, position, velocity, accel, start_scale, end_scale, start_rot, end_rot, start_col, end_col);
}

void ParticleLoad::SetCamera(Camera* camera) {
	this->camera = camera;

	for (auto& i : particles) {
		i->SetCamera(camera);
	}
}

