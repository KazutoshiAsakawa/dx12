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

	/// <summary>
	/// �ŏ��̃V�[���J��
	/// </summary>
	void start();

	/// <summary>
	/// �v���C���
	/// </summary>
	void play();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// �O�i�X�v���C�g�`��
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;

	/// <summary>
	/// �X�v���C���Ȑ�
	/// </summary>
	/// <param name="posints">���W</param>
	/// <param name="startIndex">�n�_</param>
	/// <param name="t">����</param>
	/// <returns></returns>
	DirectX::XMVECTOR SplinePosition(const std::vector<DirectX::XMVECTOR>& posints, size_t startIndex, float t);

	// �G�𔭐�������
	std::unique_ptr<Enemy>& EnemyAdd(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel);

	/// <summary>
	/// �_���[�W�G�t�F�N�g
	/// </summary>
	/// <param name="maxFrame">�ő�t���[��</param>
	/// <param name="nowFrame">���݂̃t���[��</param>
	void DamageEffect(UINT maxFrame,UINT nowFrame);

private:
	// �w�i�X�v���C�g
	std::unique_ptr<Sprite> sprite;
	// �Ə��X�v���C�g
	std::unique_ptr<Sprite> aim;

	// �̗̓o�[
	std::unique_ptr<Sprite> hp3;
	std::unique_ptr<Sprite> hp2;
	std::unique_ptr<Sprite> hp1;

	// �G���������Ė������̃^�[�Q�b�g
	std::unique_ptr<GameObject> nullTarget;

	// �X�J�C�h�[��
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// �n��
	std::unique_ptr<ObjModel> groundModel;
	std::unique_ptr<ObjObject3d> groundObj;

	// �e
	std::unique_ptr<ObjModel> pBulletModel = nullptr;
	// �e�̑傫��
	float pBulletScale = 1;

	// �G
	std::unique_ptr<ObjModel> enemyModel = nullptr;
	// �G�̑傫��
	float enemyScale = 2;
	// �J����
	std::unique_ptr<TrackingCamera> camera;

	// ���[���̐^��
	std::unique_ptr<GameObject> lane;

	// �v���C���[
	std::unique_ptr<Player> player;
	// ����̃N�[���^�C��
	UINT avoidFrame = 0;
	UINT avoidFrameMax = 60;

	// �G
	std::list<std::unique_ptr<Enemy>> enemy;

	// �V�[���J��
	std::function<void()> updateProcess;

	// ���U�C�N
	bool mosaicFlag = false;
	UINT mosaicFrame = 0;

	// RGB���炵
	UINT nowFrame = 0;
	UINT maxFrame = 15;
	bool shiftFlag = false;

	std::vector<DirectX::XMVECTOR> points;
	size_t splineStartIndex;

	UINT frame = 0;

	// �G�𔭐�
	// �n�܂�,�I���
	std::list<std::pair<UINT, UINT>> enemyFrame;

	std::vector< std::vector<std::string>> csv;

	// �G�̍��W
	std::vector<DirectX::XMFLOAT3> enemyPos;

	// ���₵���G�̐�
	UINT addedEnemyNum = 0;

	// �|�[�Y��ʊǗ�
	bool pause = false;


};