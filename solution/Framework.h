#pragma once

#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "ObjObject3d.h"
#include "ObjModel.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Audio.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#include "PostEffect.h"

#include<memory>

// �t���[�����[�N
class Framework
{
public:
	/// <summary>
	/// ���s
	/// </summary>
	void Run();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// �I��
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw();

	virtual bool IsEndRequest() { return endRequest_; }

protected:
	bool endRequest_ = false;
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;
	DebugText* debugText = nullptr;
	Audio* audio = nullptr;
	Input* input = nullptr;
	SpriteCommon* spriteCommon = nullptr;
	// ���L��(�Ǘ���)
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

	UINT shaderNum = 0u;
};

