#include <SceneFactory.h>
#include <ResourceManager.h>
#include <GameObject.h>
#include <SkinnedRenderer.h>
#include <Animator.h>
#include <Camera.h>

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
		animator->RegistAnimation("IdleC", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleC").get());animator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Walk").get());
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
	}
}