#include "Player.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Input.h"

using namespace DirectX;

Player::Player()
	:Player(ObjModel::LoadFromObj("fox"))
{
	// �v���C���[�̃I�u�W�F�N�g
	obj->SetPosition({ 0,0,0 });
	obj->SetScale({ 1,1,1 });

	// �Ə��̃I�u�W�F�N�g
	aim = ObjObject3d::Create();
	aim->Initialize();
	constexpr float aimLength = 10.f;
	aim->SetPosition({ 0,0,aimLength });
	aim->SetScale({ 0.5,0.5,1 });
	aim->SetParent(obj.get());

	// �g�t�̃I�u�W�F�N�g
	momijiObj = ObjObject3d::Create();
	momijiObj->Initialize();
	// ���f�����Z�b�g
	momijiObj->SetModel(ObjModel::LoadFromObj("MomijiLeave"));
	// �ʒu
	momijiObj->SetPosition({ 0.f, -obj->GetScale().y, 0.f });
	// �傫��
	constexpr float momijiScale = 2.f;
	momijiObj->SetScale({ momijiScale, momijiScale, momijiScale });
	// ��]
	momijiObj->SetRotation({ 0.f, 180.f, 0.f });
	// �{�̂�e�Ƃ���
	momijiObj->SetParent(obj.get());

	// �U���Ώۂւ̃|�C���^
	shotTarget = nullptr;

	// �̗͂�������
	hp = 1;
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
	aim->Update(); // �v���C���[�̖ڂ̑O�ɂ��邽���̃��f��
	momijiObj->Update();
}

void Player::Draw()
{
	for (auto& i : bullet)
	{
		i.Draw();
	}

	if (alive) {
		obj->Draw();
		momijiObj->Draw();
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
			shotTarget->GetPosition().x - obj->GetPosition().x,
			shotTarget->GetPosition().y - obj->GetPosition().y,
			shotTarget->GetPosition().z - obj->GetPosition().z
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
