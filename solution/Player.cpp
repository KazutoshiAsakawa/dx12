#include "Player.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Input.h"

using namespace DirectX;

Player::Player()
	:Player(ObjModel::LoadFromObj("rat"))
{
	obj->SetPosition({ 0,0,0 });
	obj->SetScale({ 1,1,1 });

	aim = ObjObject3d::Create();
	aim->Initialize();
	aim->SetModel(ObjModel::LoadFromObj("sphere"));
	constexpr float aimLength = 10;
	aim->SetPosition({ 0,0,aimLength });
	aim->SetScale({ 0.5,0.5,1 });
	aim->SetParent(obj.get());

	shotTarget = nullptr;
}

void Player::Update()
{
	{
		XMMATRIX mat =
			obj->GetCamera()->GetViewMatrix() *
			obj->GetCamera()->GetProjectionMatrix() *
			obj->GetCamera()->GetViewPortMatrix();

		XMVECTOR screenAimPos = XMVector3Transform(aim->GetMatWorld().r[3], mat);
		screenAimPos /= XMVectorGetW(screenAimPos);

		// �ϐ��Ɋi�[
		float2ScreenAimPos = XMFLOAT2(XMVectorGetX(screenAimPos), XMVectorGetY(screenAimPos));
	}

	for (auto& i : bullet)
	{
		i.Update();
	}

	bullet.erase(std::remove_if(bullet.begin(), bullet.end(), [](PlayerBullet& i) {return !i.GetAlive(); }), bullet.end());

	obj->Update();
	aim->Update();
}

void Player::Draw()
{
	for (auto& i : bullet)
	{
		i.Draw();
	}

	if (alive) {
		obj->Draw();
		aim->Draw();
	}

}

void Player::Shot(ObjModel* model, float scale)
{
	bullet.emplace_back(model, obj->GetPosition());
	bullet.back().SetScale({ scale,scale,scale });
	bullet.back().SetParent(obj->GetParent());
	XMFLOAT3 vel;
	constexpr float velScale = 2.f;

	XMStoreFloat3(&vel, XMVector3Transform(XMVectorSet(0, 0, velScale, 1), obj->GetMatRot()));

	if (shotTarget != nullptr) {
		// ���x���v�Z
		// ��������W�I�܂ł̃x�N�g��
		vel = {
			shotTarget->GetPos().x - obj->GetPosition().x,
			shotTarget->GetPos().y - obj->GetPosition().y,
			shotTarget->GetPos().z - obj->GetPosition().z
		};
		// XMVECTOR�ɕϊ�
		XMVECTOR vectorVel = XMLoadFloat3(&vel);
		// �傫����1�ɂ���
		vectorVel = XMVector3Normalize(vectorVel);
		// �傫����C�ӂ̒l�ɂ���
		vectorVel = XMVectorScale(vectorVel, velScale);
		// FLOAT3�ɕϊ�
		XMStoreFloat3(&vel, vectorVel);

		// �W�I�Ɍ�����
		float rotx = atan2f(vel.y, vel.z);
		float roty = atan2f(vel.x, vel.z);
		bullet.back().SetRotation(XMFLOAT3(XMConvertToDegrees(rotx), XMConvertToDegrees(roty), 0));
	}

	bullet.back().SetVel(vel);
}
