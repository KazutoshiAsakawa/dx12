#include "BaseScene.h"
#include "LightGroup.h"
#include "ObjObject3d.h"

void BaseScene::InitializeScene() {
	lightGroup = std::make_unique<LightGroup>();
	ObjObject3d::SetLightGroup(lightGroup.get());
	Initialize(DirectXCommon::GetInstance());
}

void BaseScene::FinalizeScene() {
	Finalize();
	ObjObject3d::SetLightGroup(nullptr);
}
