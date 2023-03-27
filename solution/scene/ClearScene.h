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

#include <Vector>
#include <memory>
#include <functional>
#include <unordered_map>

/// <summary>
/// �Q�[���v���C�V�[��
/// </summary>
class ClearScene : public BaseScene {
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
	// std::unique_ptr<Player> player;
	std::unique_ptr<ObjObject3d> player;

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
	std::unordered_map<std::string, std::unique_ptr<Sprite>> gameOver;

	bool eyeControl = false;

	float rad = 0;
};