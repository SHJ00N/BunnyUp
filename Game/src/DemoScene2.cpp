#include <EngineSystem.h>

#include "DemoScene2.h"

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

        for (int i = 0; i < 10; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                for (int k = 0; k < 10; ++k)
                {
                    auto cube = CreateGameObject<GameObject>("Cube" + std::to_string(i * 10 + j * 10 + k));
                    auto cubeRenderer = cube->AddComponent<MeshRenderer>();
                    cubeRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
                    cube->transform.SetLocalPosition(Vector3(i * 5.0f, k * 5.0f, j * 5.0f));
                    cube->AddComponent<BoxCollider>();
                }
            }
        }
	}
}