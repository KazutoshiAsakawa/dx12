#pragma once
#include "Camera.h"
#include "GameObject.h"

class TrackingCamera :
	public Camera
{
public:
	using Camera::Camera;

	// �R���X�g���N�^
	TrackingCamera();

	inline void SetTrackingTarget(GameObject* trackingTarget) { this->trackingTarget = trackingTarget; };

	void startUpdate()override;

	inline void SetAngle(const XMFLOAT3& angle){ this->angle = angle; }
	inline XMFLOAT3 GetAngle() { return angle; }

private:
	// �g���b�L���O����W�I
	GameObject* trackingTarget;

	XMFLOAT3 trackingTargetToCameraTarget = { 0.f,2.5f,0.f };
	XMFLOAT3 angle = {20.f, 0.f, 0.f};

	// �J�����̎��_���璍���_�܂ł̋���
	float eyeToCameraTargetLength = 10;


};

