#pragma once

#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// �V�[���Ǘ�
/// </summary>
class SceneManager
{
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

	void SetSceneFactory(AbstractSceneFactory* sceneFactory){ sceneFactory_  = sceneFactory;}
private:

	// �V�[��
	BaseScene* scene_ = nullptr;
	// ���̃V�[��
	BaseScene* nextScene_ = nullptr;
	// �V�[���t�@�N�g���[(�؂�Ă���)
	AbstractSceneFactory* sceneFactory_ = nullptr;
	
	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	void operator = (const SceneManager&) = delete;
};

