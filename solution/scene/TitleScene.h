#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "ObjObject3d.h"
#include "FbxObject3d.h"

#include "DebugCamera.h"
#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "TrackingCamera.h"
#include "Boss.h"
#include "LightGroup.h"

#include <Vector>
#include <memory>
#include <functional>
#include <unordered_map>

/// <summary>
/// �Q�[���v���C�V�[��
/// </summary>
class TitleScene : public BaseScene {
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(DirectXCommon* dxcommon) override;

	/// <summary>
	/// �I������
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �J�n
	/// </summary>
	void start();

	/// <summary>
	/// �Q�[���{��
	/// </summary>
	void play();

	/// <summary>
	///	�I������Ƃ��̉��o
	/// </summary>
	/// <param name="nextScene">���̃V�[��</param>
	void end(const std::string& nextScene);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// �O�i�X�v���C�g�`��
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;

private:
	Input* input = nullptr;

	// �X�J�C�h�[��
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// �n��
	std::unique_ptr<ObjModel> groundModel;
	std::unique_ptr<ObjObject3d> groundObj;

	// �J����
	std::unique_ptr<DebugCamera> camera;

	// �v���C���[
	std::unique_ptr<Player> player;

	std::function<void()> updateProcess;

	// �_��
	std::unique_ptr<ObjObject3d> shrineObj;

	// ⾉�
	std::unique_ptr<ObjObject3d> bonfireR;
	std::unique_ptr<ObjObject3d> bonfireL;

	// ���U�C�N
	bool mosaicFlag = false;
	UINT mosaicFrame = 0;

	std::vector<DirectX::XMVECTOR> points;
	size_t splineStartIndex;

	// �t���[��
	UINT frame;

	// �^�C�g�����
	std::unordered_map<std::string, std::unique_ptr<Sprite>> title;

	bool eyeControl = false;

	float rad = 0;
	DirectX::XMFLOAT3 ambientColor0 = { 1,1,1 };
	// �������������l
	DirectX::XMFLOAT3 lightDir0 = { 0,0,1 };
	DirectX::XMFLOAT3 lightColor0 = { 1,0,0 };

	DirectX::XMFLOAT3 lightDir1 = { 0,1,0 };
	DirectX::XMFLOAT3 lightColor1 = { 0,1,0 };

	DirectX::XMFLOAT3 lightDir2 = { 1,0,0 };
	DirectX::XMFLOAT3 lightColor2 = { 0,0,1 };

	DirectX::XMFLOAT3 pointLightPos = { 0,0,0 };
	DirectX::XMFLOAT3 pointLightColor = { 1,1,1 };
	DirectX::XMFLOAT3 pointLightAtten = { 0.3f,0.1f,0.1f };

	DirectX::XMFLOAT3 spotLightDir = { 0,-1,0 };
	DirectX::XMFLOAT3 spotLightPos = { 0,5,0 };
	DirectX::XMFLOAT3 spotLightColor = { 1,1,1 };
	DirectX::XMFLOAT3 spotLightAtten = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT2 spotLightFactorAngle = { 20.0f, 30.0f };

	DirectX::XMFLOAT3 circleShadowDir = { 0,-1,0 };
	DirectX::XMFLOAT3 circleShadowAtten = { 0.5f,0.6f,0.0f };
	DirectX::XMFLOAT2 circleShadowFactorAngle = { 0.0f, 0.5f };
};