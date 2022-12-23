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

	inline void SetAngle(const XMFLOAT3& angle) { this->angle = angle; }
	inline XMFLOAT3 GetAngle() { return angle; }

	/// <summary>
	/// カメラの視点から注視点までの距離をセット
	/// </summary>
	/// <param name="length">距離</param>
	inline void SetEyeToCameraTargetLength(float length) { this->eyeToCameraTargetLength = length; }
	/// <summary>
	/// カメラの視点から注視点までの距離
	/// </summary>
	/// <returns>距離</returns>
	inline float GetEyeToCameraTargetLength() { return eyeToCameraTargetLength; }

private:
	// トラッキングする標的
	GameObject* trackingTarget;

	XMFLOAT3 trackingTargetToCameraTarget = { 0.f,2.5f,0.f };
	XMFLOAT3 angle = { 20.f, 0.f, 0.f };

	// カメラの視点から注視点までの距離
	float eyeToCameraTargetLength = 10;


};

