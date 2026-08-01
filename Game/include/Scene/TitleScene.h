#pragma once

#include "Scene.h"

namespace Game
{
	using namespace Engine;
	class TitleScene : public Scene
	{
	public:
        TitleScene();
		~TitleScene();

    protected:
        void SceneEnter() override;
	};
}