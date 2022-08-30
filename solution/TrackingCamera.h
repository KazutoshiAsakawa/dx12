#pragma once
#include "Camera.h"
#include "GameObject.h"

class TrackingCamera :
	public Camera
{
public:
	using Camera::Camera;

	// コンストラクタ
	TrackingCamera();

	inline void SetTrackingTarget(GameObject* trackingTarget) { this->trackingTarget = trackingTarget; };

	void startUpdate()override;

private:
	GameObject* trackingTarget;

	// トラッキングターゲットからのカメラの高さ
	float trackingTargetToCameraTargetHeight = 5;

	XMFLOAT3 trackingTargetToCameraTarget = { 0,5,0 };

	// カメラの視点から注視点までの距離
	float eyeToCameraTargetLength = 20;


};

