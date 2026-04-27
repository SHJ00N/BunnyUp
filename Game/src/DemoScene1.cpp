#include <SceneFactory.h>
#include <ResourceManager.h>
#include <GameObject.h>
#include <SkinnedRenderer.h>
#include <Animator.h>

#include "DemoScene1.h"

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
		bunny->AddComponent<SkinnedRenderer>()->SetModel(ResourceManager::GetInstance().GetModel("Chibi_Rabbit"));

		auto model = ResourceManager::GetInstance().GetModel("Chibi_Rabbit");

		auto animator = bunny->AddComponent<Animator>();
		animator->Awake();

		animator->RegistAnimation("IdleA", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleA").get());
		animator->RegistAnimation("IdleC", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleC").get());
		animator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Walk").get());
		animator->RegistAnimation("Run", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Run").get());
		animator->PlayAnimation("IdleA");
	}
}