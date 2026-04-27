#pragma once

#include "Scene.h"

namespace Scenes
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