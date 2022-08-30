#include "TrackingCamera.h"
#include "WinApp.h"

#include <DirectXMath.h>

using namespace DirectX;

TrackingCamera::TrackingCamera() :Camera(WinApp::window_width, WinApp::window_height)
{

}

void TrackingCamera::startUpdate()
{

	// ƒgƒ‰ƒbƒLƒ“ƒOƒ^[ƒQƒbƒg‚ª‘¶Ý‚µ‚½‚ç
	if (trackingTarget) {
		XMFLOAT3 target = trackingTarget->GetPos();
		// target.y += trackingTargetToCameraTargetHeight;

		float sinNum = sinf(XMConvertToRadians(trackingTarget->GetRotation().x + 30) );
		float cosNum = cosf(XMConvertToRadians(trackingTarget->GetRotation().x + 30));

		// xŽ²‰ñ“]‚ð”½‰f‚µ‚½ˆÊ’u
		XMFLOAT3 tempPosition = { 0,sinNum * eyeToCameraTargetLength ,-cosNum * eyeToCameraTargetLength };

		sinNum = sinf(XMConvertToRadians(-trackingTarget->GetRotation().y));
		cosNum = cosf(XMConvertToRadians(-trackingTarget->GetRotation().y));

		// yŽ²‰ñ“]‚ð”½‰f‚µ‚½ˆÊ’u
		XMFLOAT3 tempPosition2 = {
			cosNum * tempPosition.x - sinNum * tempPosition.z,
			tempPosition.y,
			sinNum * tempPosition.x + cosNum * tempPosition.z
		};

		XMFLOAT3 eye = {
		target.x + tempPosition2.x,
		target.y + tempPosition2.y,
		target.z + tempPosition2.z };

		SetEye(eye);
		 SetTarget(target);
	}
}
