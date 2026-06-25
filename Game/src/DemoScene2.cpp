#include <EngineSystem.h>

#include "DemoScene2.h"
#include "CubeController.h"
#include "CubeGenerator.h"

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

        auto floor = CreateGameObject<GameObject>("Floor");
        auto floorRenderer = floor->AddComponent<MeshRenderer>();
        floorRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_quad"));
        floorRenderer->SetMaterial(0, ResourceManager::GetInstance().GetMaterial("Textured_material"));
        floorRenderer->GetMaterial(0)->SetTexture(0, ResourceManager::GetInstance().GetTexture("T_Snow_Ground"));
        floorRenderer->GetMaterial(0)->SetSampler(0, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));

        floor->transform.SetLocalRotation(Vector3(90.0f, 0.0f, 0.0f));
        floor->transform.SetLocalScale(Vector3(500.0f, 500.0f, 1.0f));
        floor->transform.SetLocalPosition(Vector3(0.0f, -50.0f, 0.0f));

        auto floorRigidbody = floor->AddComponent<Rp3dRigidbody>(1.0f, BodyType::STATIC, false);
        auto floorCollider = floor->AddComponent<Rp3dBoxCollider>();
        floorCollider->SetSize(Vector3(500.0f, 500.0f, 1.0f));

        auto cubeGenerator = CreateGameObject<GameObject>("CubeGenerator");
        cubeGenerator->AddComponent<Cube::CubeGenerator>();
	}
}