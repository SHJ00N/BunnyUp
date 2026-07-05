#pragma once

#include "Scene.h"

namespace Game
{
	using namespace Engine;
	class DemoScene2 : public Scene
	{
	public:
		DemoScene2();
		~DemoScene2();

    protected:
        void SceneEnter() override;
	};
}