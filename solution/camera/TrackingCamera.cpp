#include "TrackingCamera.h"
#include "WinApp.h"

#include <DirectXMath.h>

using namespace DirectX;

TrackingCamera::TrackingCamera() :Camera(WinApp::window_width, WinApp::window_height)
{

}

void TrackingCamera::startUpdate()
{

	// トラッキングターゲットが存在したら
	if (trackingTarget) {
		XMFLOAT3 target = trackingTarget->GetPosition();

		ObjObject3d* targetParent = trackingTarget->GetParent();
		while (targetParent) {
			target.x += targetParent->GetPosition().x;
			target.y += targetParent->GetPosition().y;
			target.z += targetParent->GetPosition().z;

			targetParent = targetParent->GetParent();
		}

		float sinNum = sinf(XMConvertToRadians(trackingTarget->GetRotation().x + angle.x));
		float cosNum = cosf(XMConvertToRadians(trackingTarget->GetRotation().x + angle.x));

		// x軸回転を反映した位置
		XMFLOAT3 tempPosition = { 0,sinNum * eyeToCameraTargetLength ,-cosNum * eyeToCameraTargetLength };

		sinNum = sinf(XMConvertToRadians(-trackingTarget->GetRotation().y + angle.y));
		cosNum = cosf(XMConvertToRadians(-trackingTarget->GetRotation().y + angle.y));

		// y軸回転を反映した位置
		XMFLOAT3 tempPosition2 = {
			cosNum * tempPosition.x - sinNum * tempPosition.z,
			tempPosition.y,
			sinNum * tempPosition.x + cosNum * tempPosition.z
		};

		XMFLOAT3 eye = {
		target.x + tempPosition2.x,
		target.y + tempPosition2.y,
		target.z + tempPosition2.z };

		// 移動前の座標
		XMFLOAT3 old = GetEye();
		// 移動幅 = 移動後の座標 - 移動前の座標
		XMFLOAT3 vel =
		{ (eye.x - old.x) * 0.2f,
		(eye.y - old.y) * 0.2f,
		(eye.z - old.z) * 0.2f };
		// 移動後の座標 = 移動前の座標 + 移動幅
		eye = { old.x + vel.x,old.y + vel.y ,old.z + vel.z };
		// 移動後の座標を適用
		SetEye(eye);


		XMFLOAT3 targetVel;
		XMStoreFloat3(&targetVel, XMVector3Transform(XMVectorSet(
			trackingTargetToCameraTarget.x,
			trackingTargetToCameraTarget.y,
			trackingTargetToCameraTarget.z, 1),
			trackingTarget->GetMatRotation()));

		target.x += targetVel.x;
		target.y += targetVel.y;
		target.z += targetVel.z;
		SetTarget(target);


	}
}
