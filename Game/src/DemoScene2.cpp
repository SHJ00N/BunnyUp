#include <SceneFactory.h>
#include <ResourceManager.h>
#include <GameObject.h>
#include <SkinnedRenderer.h>
#include <Animator.h>

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
		auto bunny = CreateGameObject<GameObject>("Bunny");
		bunny->transform.SetLocalScale(Vector3(0.085f, 0.085f, 0.085f));
		auto renderer = bunny->AddComponent<SkinnedRenderer>();
		renderer->SetModel(ResourceManager::GetInstance().GetModel("Chibi_Rabbit"));
		auto& materials = renderer->GetMaterials();
		materials[0].get()->SetTexture(0, ResourceManager::GetInstance().GetTexture("T_Chibi_Rabbit_03"));


		auto animator = bunny->AddComponent<Animator>();
		animator->Awake();

		auto model = ResourceManager::GetInstance().GetModel("Chibi_Rabbit");
		animator->RegistAnimation("IdleA", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleA").get());
		animator->RegistAnimation("IdleC", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleC").get());
		animator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Walk").get());
		animator->RegistAnimation("Run", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Run").get());
		animator->PlayAnimation("IdleC");
	}
}