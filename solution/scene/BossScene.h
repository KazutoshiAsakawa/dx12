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
class BossScene : public BaseScene {
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
	void Start();

	/// <summary>
	/// �{�X�o��
	/// </summary>
	void BossEntry();

	/// <summary>
	/// �Q�[���{��
	/// </summary>
	void Play();

	/// <summary>
	/// �{�X��|�����G�t�F�N�g
	/// </summary>
	void KillEffect();

	/// <summary>
	/// �v���C���[�̎��S���o
	/// </summary>
	void DeathPlayer();

	/// <summary>
	///	�I������Ƃ��̉��o
	/// </summary>
	/// <param name="nextScene">���̃V�[��</param>
	void End(const std::string& nextScene);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(DirectXCommon* dxcommon) override;

	/// <summary>
	/// �O�i�X�v���C�g�`��
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon) override;


	DirectX::XMVECTOR SplinePosition(const std::vector<DirectX::XMVECTOR>& posints, size_t startIndex, float t);

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

	// �}�E�X
	POINT mousePosDiff{};

	// �Ə��X�v���C�g
	std::unique_ptr<Sprite> aim;

	// ����摜
	std::unordered_map<std::string, std::unique_ptr<Sprite>> operationSprite;

	// �X�J�C�h�[��
	std::unique_ptr<ObjModel> skyDomeModel;
	std::unique_ptr<ObjObject3d> skyDomeObj;

	// �n��
	std::unique_ptr<ObjModel> groundModel;
	std::unique_ptr<ObjObject3d> groundObj;

	// �|�[�Y��ʂ̖���
	UINT pauseMax = 3;
	// �|�[�Y��ʂ̔z��
	std::vector<std::unique_ptr<Sprite>> pauseSprite;
	int pause = 0;

	// ���@HP�ő�l
	UINT playerHpMax = 30;
	// HP�摜
	DirectX::XMFLOAT2 playerHpSpriteSize;
	std::unique_ptr<Sprite> playerHpSprite;
	std::unique_ptr<Sprite> playerHpSlide;

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
	float cameraLengthDef;

	// �v���C���[
	std::unique_ptr<Player> player;
	// ����̃N�[���^�C��
	UINT avoidFrame = 0;
	UINT avoidFrameMax = 60;

	UINT shotInterval = 0;
	UINT shotIntervalMax = 15;

	// �{�X
	std::unique_ptr<Boss> boss;
	// �{�XHP�ő�l
	UINT bossHpMax = 30;
	float bossScale;
	// �{�XHP�摜
	DirectX::XMFLOAT2 bossHpSpriteSize;
	std::unique_ptr<Sprite> bossHpSprite;
	std::unique_ptr<Sprite> bossHpSlide;

	// �{�X�̖��O�摜
	std::unique_ptr<Sprite> bossText;


	bool killBossFlag = false;

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

	UINT nowEffectFrame = 0;

	UINT bossEntryNowFrame = 0;

	std::string bgmFileName;
};