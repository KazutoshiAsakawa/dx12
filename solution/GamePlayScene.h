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

#include <Vector>
#include <memory>
#include <functional>

/// <summary>
/// �Q�[���v���C�V�[��
/// </summary>
class GamePlayScene : public BaseScene
{
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

	void start();
	void play();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// �O�i�X�v���C�g�`��
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;


	DirectX::XMVECTOR splinePosition(const std::vector<DirectX::XMVECTOR>& posints, size_t startIndex, float t);

	// �G�𔭐�������
	std::unique_ptr<Enemy>& enemyAdd(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel);

	

private:
	// �w�i
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Sprite> aim;

	// �X�J�C�h�[��
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// �e
	std::unique_ptr<ObjModel> pBulletModel = nullptr;
	// �e�̑傫��
	float pBulletScale = 2;

	// �G
	std::unique_ptr<ObjModel> enemyModel = nullptr;
	// �G�̑傫��
	float enemyScale = 2;
	// �J����
	std::unique_ptr<TrackingCamera> camera;

	// �v���C���[
	std::unique_ptr<Player> player;
	std::list<std::unique_ptr<Enemy>> enemy;

	std::function<void()> updateProcess;

	// ���U�C�N
	bool mosaicFlag = false;
	UINT mosaicFrame = 0;

	std::vector<DirectX::XMVECTOR> points;
	size_t splineStartIndex;

	UINT frame = 0;

	// �G�𔭐�
	// �n�܂�,�I���
	std::list<std::pair<UINT, UINT>> enemyFrame;


	std::vector< std::vector<std::string>> csv;

	std::vector<DirectX::XMFLOAT3> enemyPos;

	UINT addedEnemyNum = 0;
};