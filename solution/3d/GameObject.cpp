#include "GameObject.h"

using namespace DirectX;

GameObject::GameObject(ObjModel* model, const DirectX::XMFLOAT3& position) :obj(std::move(ObjObject3d::Create()))
{
	obj->SetModel(model);
	obj->SetPosition(position);

}

void GameObject::Update()
{

	Screen();

	obj->Update();
}

void GameObject::Draw()
{
	if (alive) {
		obj->Draw();
	}
}

void GameObject::Screen()
{
	{
		XMMATRIX mat =
			obj->GetCamera()->GetViewMatrix() *
			obj->GetCamera()->GetProjectionMatrix() *
			obj->GetCamera()->GetViewPortMatrix();

		XMVECTOR screenPos = XMVector3Transform(obj->GetMatWorld().r[3], mat);
		screenPos /= XMVectorGetW(screenPos);

		// •Ï”‚ÉŠi”[
		float2ScreenPos = XMFLOAT2(XMVectorGetX(screenPos), XMVectorGetY(screenPos));
	}
}