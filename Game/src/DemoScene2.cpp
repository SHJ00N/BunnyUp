#include <EngineSystem.h>

#include "DemoScene2.h"
#include "CubeController.h"

namespace Scenes
{
	DemoScene2::DemoScene2()
	{
	}

	DemoScene2::~DemoScene2()
	{
	}

	void DemoScene2::SceneEnter()
	{
        SetEnvironmentMap(ResourceManager::GetInstance().GetEnvironmentMap("Sky_EnvMap"));
	}
}