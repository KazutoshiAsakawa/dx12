#include "GameObject.h"

GameObject::GameObject(ObjModel* model, const DirectX::XMFLOAT3& position) :obj(std::move(ObjObject3d::Create()))
{
	obj->SetModel(model);
	obj->SetPosition(position);

}

void GameObject::Update()
{
	obj->Update();
}

void GameObject::Draw()
{
	if (alive) {
		obj->Draw();
	}
}
