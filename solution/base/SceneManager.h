#pragma once

#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// �V�[���Ǘ�
/// </summary>
class SceneManager {
public:
	static SceneManager* GetInstance();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update(DirectXCommon* dxcommon);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(DirectXCommon* dxcommon);

	/// <summary>
	/// �O�i�X�v���C�g�`��
	/// </summary>
	void DrawFrontSprite(DirectXCommon* dxcommon);

	/// <summary>
	/// ���V�[���\��
	/// </summary>
	/// <param name="sceneName">�V�[����</param>
	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory = sceneFactory; }
private:

	// �V�[��
	BaseScene* scene = nullptr;
	// ���̃V�[��
	BaseScene* nextScene = nullptr;
	// �V�[���t�@�N�g���[(�؂�Ă���)
	AbstractSceneFactory* sceneFactory = nullptr;

	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	void operator = (const SceneManager&) = delete;
};

