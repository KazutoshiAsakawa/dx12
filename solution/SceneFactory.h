#pragma once
#include "AbstractSceneFactory.h"

// �Q�[���p�̃V�[���H��
class SceneFactory :
    public AbstractSceneFactory
{
public:
	/// <summary>
	/// �V�[������
	/// </summary>
	/// <param name="sceneName">�V�[����</param>
	/// <returns>���������V�[��</returns>
	BaseScene* CreateScene(const std::string& sceneName) override;
};

