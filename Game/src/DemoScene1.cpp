#include <SceneFactory.h>
#include <ResourceManager.h>
#include <GameObject.h>
#include <SkinnedRenderer.h>
#include <Animator.h>
#include <Camera.h>
#include <SamplerStateManager.h>

#include "DemoScene1.h"
#include "CameraController.h"

namespace Scenes
{
	DemoScene1::DemoScene1()
	{
	}

	DemoScene1::~DemoScene1()
	{
	}

	void DemoScene1::SceneEnter()
	{
		auto bunny = CreateGameObject<GameObject>("Bunny");
		bunny->transform.SetLocalScale(Vector3(0.085f, 0.085f, 0.085f));
		bunny->transform.SetLocalRotation(Vector3(0.0f, 180.0f, 0.0f));
		bunny->AddComponent<SkinnedRenderer>()->SetModel(ResourceManager::GetInstance().GetModel("Chibi_Rabbit"));

		auto model = ResourceManager::GetInstance().GetModel("Chibi_Rabbit");

		auto animator = bunny->AddComponent<Animator>();
		animator->Awake();

		animator->RegistAnimation("IdleA", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleA").get());
		animator->RegistAnimation("IdleC", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleC").get());
        animator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Walk").get());
		animator->RegistAnimation("Run", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Run").get());
		animator->PlayAnimation("IdleA");

		auto camera1 = CreateGameObject<GameObject>("Camera1");
		camera1->AddComponent<Camera>();
		camera1->transform.SetLocalPosition(Vector3(0.0f, 100.0f, 0.0f));
		camera1->transform.SetLocalRotation(Vector3(90.0f, 0.0f, 0.0f));

		auto camera2 = CreateGameObject<GameObject>("Camera2");
		camera2->AddComponent<Camera>();
		camera2->AddComponent<CameraController>();
		camera2->transform.SetLocalPosition(Vector3(0.0f, 10.0f, -30.0f));

        auto floor = CreateGameObject<GameObject>("Floor");
        auto floorRenderer = floor->AddComponent<MeshRenderer>();
        floorRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_quad"));
        floorRenderer->SetMaterial(0, ResourceManager::GetInstance().GetMaterial("Textured_material"));
        floorRenderer->GetMaterial(0)->SetTexture(0, ResourceManager::GetInstance().GetTexture("T_Snow_Ground"));
        floorRenderer->GetMaterial(0)->SetSampler(0, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));

        floor->transform.SetLocalRotation(Vector3(90.0f, 0.0f, 0.0f));
        floor->transform.SetLocalScale(Vector3(50.0f, 50.0f, 1.0f));

        auto cube1 = CreateGameObject<GameObject>("Cube1");
        auto cube1Renderer = cube1->AddComponent<MeshRenderer>();
        cube1Renderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));

        cube1->transform.SetLocalPosition(Vector3(20.0f, 2.5f, 10.0f));
        cube1->transform.SetLocalScale(Vector3(5.0f, 5.0f, 5.0f));

        auto cube2 = CreateGameObject<GameObject>("Cube2");
        auto cube2Renderer = cube2->AddComponent<MeshRenderer>();
        cube2Renderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        cube2Renderer->GetMaterial(0)->SetColor(Vector4(0.5f, 0.5f, 0.2f, 1.0f));

        cube2->transform.SetLocalPosition(Vector3(-20.0f, 2.5f, -10.0f));
        cube2->transform.SetLocalScale(Vector3(7.5f, 7.5f, 7.5f));

        auto cube3 = CreateGameObject<GameObject>("Cube3");
        auto cube3Renderer = cube3->AddComponent<MeshRenderer>();
        cube3Renderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        cube3Renderer->GetMaterial(0)->SetColor(Vector4(0.2f, 0.7f, 0.4f, 1.0f));

        cube3->transform.SetLocalPosition(Vector3(-10.0f, 5.0f, 10.0f));
        cube3->transform.SetLocalScale(Vector3(5.0f, 5.0f, 5.0f));
	}
}