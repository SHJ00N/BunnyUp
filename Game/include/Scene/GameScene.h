#pragma once

#include "Scene.h"

namespace Game
{
	using namespace Engine;
	class GameScene : public Scene
	{
	public:
        GameScene();
		~GameScene();

	protected:
		void SceneEnter() override;

    private:
        void createTree(GameObject* parent, const std::string& name, const Vector3& pos, const Vector3& scale);
        void createGift(GameObject* parent, const std::string& name, int type, const Vector3& pos, const Vector3& rot, const Vector3& scale);
	};
}