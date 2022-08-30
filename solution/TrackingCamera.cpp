#include "TrackingCamera.h"
#include "WinApp.h"

#include <DirectXMath.h>

using namespace DirectX;

TrackingCamera::TrackingCamera() :Camera(WinApp::window_width, WinApp::window_height)
{

}

void TrackingCamera::startUpdate()
{

	// �g���b�L���O�^�[�Q�b�g�����݂�����
	if (trackingTarget) {
		XMFLOAT3 target = trackingTarget->GetPos();

		float sinNum = sinf(XMConvertToRadians(trackingTarget->GetRotation().x + 20));
		float cosNum = cosf(XMConvertToRadians(trackingTarget->GetRotation().x + 20));

		// x����]�𔽉f�����ʒu
		XMFLOAT3 tempPosition = { 0,sinNum * eyeToCameraTargetLength ,-cosNum * eyeToCameraTargetLength };

		sinNum = sinf(XMConvertToRadians(-trackingTarget->GetRotation().y));
		cosNum = cosf(XMConvertToRadians(-trackingTarget->GetRotation().y));

		// y����]�𔽉f�����ʒu
		XMFLOAT3 tempPosition2 = {
			cosNum * tempPosition.x - sinNum * tempPosition.z,
			tempPosition.y,
			sinNum * tempPosition.x + cosNum * tempPosition.z
		};

		XMFLOAT3 eye = {
		target.x + tempPosition2.x,
		target.y + tempPosition2.y,
		target.z + tempPosition2.z };

		// �ړ��O�̍��W
		XMFLOAT3 old = GetEye();
		// �ړ��� = �ړ���̍��W - �ړ��O�̍��W
		XMFLOAT3 vel =
		{ (eye.x - old.x) * 0.3f,
		(eye.y - old.y) * 0.3f,
		(eye.z - old.z) * 0.3f };
		// �ړ���̍��W = �ړ��O�̍��W + �ړ���
		eye = { old.x + vel.x,old.y + vel.y ,old.z + vel.z };
		// �ړ���̍��W��K�p
		SetEye(eye);


		XMFLOAT3 a;
		XMStoreFloat3(&a, XMVector3Transform(XMVectorSet(
			trackingTargetToCameraTarget.x,
			trackingTargetToCameraTarget.y,
			trackingTargetToCameraTarget.z, 1),
			trackingTarget->GetMatRotation()));

		target.x += a.x;
		target.y += a.y;
		target.z += a.z;
		SetTarget(target);
	}
}
