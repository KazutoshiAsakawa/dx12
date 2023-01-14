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

#include <unordered_map>

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
	/// �v���C���[�̓o�ꉉ�o
	/// </summary>
	void entryPlayer();

	/// <summary>
	/// �v���C���
	/// </summary>
	void play();


	/// <summary>
	/// �v���C���[�̑ޏꉉ�o
	/// </summary>
	void exitPlayer();


	/// <summary>
	/// �v���C���[�̎��S���o
	/// </summary>
	void deathPlayer();

	/// <summary>
	/// �I������Ƃ��̉��o
	/// </summary>
	void end(const std::string& sceneName);

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
	void DamageEffect(UINT maxFrame, UINT nowFrame);

	/// <summary>
	/// �v���C���[�̈ړ��Ɖ��
	/// </summary>
	void PlayerMove();

	/// <summary>
	/// �Ə��ƓG�̃X�N���[�����W�̓����蔻��
	/// </summary>
	void CollisionAimAndEnemyScreenPos();

	/// <summary>
	/// �G�Ǝ��@�̒e�̓����蔻��
	/// </summary>
	void CollisionEnemyAndPlayerBullet();

	/// <summary>
	/// ���@�ƓG�̒e�̓����蔻��
	/// </summary>
	void CollisionPlayerAndEnemyBullet();

private:
	Input* input = nullptr;

	// �Ə��X�v���C�g
	std::unique_ptr<Sprite> aim;

	// �|�[�Y��ʂ̖���
	UINT pauseMax = 3;
	// �|�[�Y��ʂ̔z��
	std::vector<std::unique_ptr<Sprite>> pauseSprite;
	int pause = 0;

	// ���@HP�ő�l
	UINT playerHpMax = 10;
	// HP�摜�̔z��
	DirectX::XMFLOAT2 playerHpSpriteSize;
	std::unique_ptr<Sprite> playerHpSprite;
	std::unique_ptr<Sprite> playerHpSlide;

	// ����摜
	std::unordered_map<std::string, std::unique_ptr<Sprite>> operationSprite;

	// �G���������Ė������̃^�[�Q�b�g
	std::unique_ptr<GameObject> nullTarget;

	// �X�J�C�h�[��
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// �n��
	std::unique_ptr<ObjModel> groundModel;
	std::unique_ptr<ObjObject3d> groundObj;

	// �_��
	std::unique_ptr<ObjObject3d> shrineObj;

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
	std::unique_ptr<Camera> normalCamera;
	Camera* nowCamera = nullptr;

	// ���[���̐^��
	std::unique_ptr<GameObject> lane;

	// �v���C���[
	std::unique_ptr<Player> player;
	// ����̃N�[���^�C��
	UINT avoidFrame = 0;
	UINT avoidFrameMax = 60;

	UINT shotInterval = 0;
	UINT shotIntervalMax = 15;

	// �G
	std::list<std::unique_ptr<Enemy>> enemy;

	// �V�[���J��
	std::function<void()> updateProcess;

	// ���U�C�N
	bool mosaicFlag = false;
	UINT mosaicFrame = 0;

	// �J�n���o
	UINT playerEntryFrame = 0;
	// �o�ꉉ�o�̍��W
	DirectX::XMFLOAT3 playerEntryStartPos;
	DirectX::XMFLOAT3 playerEntryEndPos;

	// �ޏꉉ�o
	UINT playerExitFrame = 0;
	// �o�ꉉ�o�̍��W
	DirectX::XMFLOAT3 playerExitStartPos;
	DirectX::XMFLOAT3 playerExitEndPos;

	// RGB���炵
	UINT shiftNowFrame = 0;
	UINT shiftMaxFrame = 15;
	bool shiftFlag = false;

	// �X�v���C���Ȑ��̓_
	std::vector<DirectX::XMVECTOR> points;
	size_t splineStartIndex;

	// ����
	std::vector<std::vector<std::unique_ptr<ObjObject3d>>> toriiObj;
	std::unique_ptr<ObjModel> toriiModel;

	// ��
	std::vector<std::vector<std::unique_ptr<ObjObject3d>>> wallObj;
	std::unique_ptr<ObjModel> wallModel;


	UINT frame = 0;

	UINT splineFrame = 0;

	// �G�𔭐�
	// �n�܂�,�I���
	std::list<std::pair<UINT, UINT>> enemyFrame;

	std::vector< std::vector<std::string>> csv;

	std::vector< std::vector<std::string>> splineCsv;

	// �G�̔������W
	std::vector<DirectX::XMFLOAT3> enemyPos;

	// ���₵���G�̐�
	UINT addedEnemyNum = 0;
};