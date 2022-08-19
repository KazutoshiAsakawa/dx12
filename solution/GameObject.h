#pragma once

#include "ObjModel.h"
#include "ObjObject3d.h"
#include <DirectXMath.h>
#include <memory>

class GameObject {

protected:
	std::unique_ptr<ObjObject3d> obj;

	bool alive = true;


public:


	inline bool GetAlive() const { return alive; }
	// alive‚ðfalse‚É‚·‚é
	inline void kill() { alive = false; }

	inline void SetPos(const DirectX::XMFLOAT3& position) { obj->SetPosition(position); }
	inline const DirectX::XMFLOAT3& GetPos() const { return obj->GetPosition(); }

	inline void SetScale(const DirectX::XMFLOAT3& scale) { obj->SetScale(scale); }
	inline const DirectX::XMFLOAT3& GetScale() const { return obj->GetScale(); }

	inline void SetRotation(const DirectX::XMFLOAT3& rotation) { obj->SetRotation(rotation); }
	inline const DirectX::XMFLOAT3& GetRotation() const { return obj->GetRotation(); }

	GameObject(
		ObjModel* model,
		const DirectX::XMFLOAT3& position = { 0,0,0 });

	virtual void Update();

	virtual void Draw();
};