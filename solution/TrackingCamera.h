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

private:
	GameObject* trackingTarget;

	// �g���b�L���O�^�[�Q�b�g����̃J�����̍���
	float trackingTargetToCameraTargetHeight = 5;

	XMFLOAT3 trackingTargetToCameraTarget = { 0,5,0 };

	// �J�����̎��_���璍���_�܂ł̋���
	float eyeToCameraTargetLength = 20;


};

