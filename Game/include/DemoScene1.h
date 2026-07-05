#pragma once

#include "Scene.h"

namespace Game
{
	using namespace Engine;
	class DemoScene1 : public Scene
	{
	public:
		DemoScene1();
		~DemoScene1();

	protected:
		void SceneEnter() override;
	};
}