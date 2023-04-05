#include "PlayerBullet.h"
#include "ParticleLoad.h"
#include "WinApp.h"

using namespace DirectX;

void PlayerBullet::Update() {
	if (++numFrame > life) {
		alive = false;
	}

	auto pos = obj->GetPosition();
	pos.x += vel.x;
	pos.y += vel.y;
	pos.z += vel.z;

	obj->SetPosition(pos);

	WinApp* winApp = WinApp::GetInstance();

	constexpr int texNum = 1;
	constexpr XMFLOAT3 accel = { 0.f,0.f,0.f };
	constexpr float startScale = { 1.f };
	constexpr XMFLOAT3 startCol = { 0.7f,0.7f,0.3f };
	constexpr XMFLOAT3 endCol = { 1.f,0.f,0.f };
	XMFLOAT3 bulletPos = obj->GetWorldPos();
	bulletPos.x += winApp->MyRandMinMax(0.f, 0.125f);
	bulletPos.z += winApp->MyRandMinMax(0.f, 0.125f);
	XMFLOAT3 velocity = { winApp->MyRand(0.f, 0.02f), 0.2f, winApp->MyRand(0.f, 0.02f) };
	ParticleLoad::GetInstance()->SetRenderAdd(texNum, winApp->MyRandMinMax(8, 24), bulletPos, velocity, accel,
		startScale, 0.f, startCol, endCol);

	obj->Update();
}
