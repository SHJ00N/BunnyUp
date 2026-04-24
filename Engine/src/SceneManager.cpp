#include "SceneManager.h"
#include "SkinnedRenderer.h"
#include "ResourceManager.h"
#include "Animator.h"

namespace Engine
{
	void SceneManager::Initialize()
	{
		auto scene = std::make_unique<Scene>();
		auto bunny = scene->CreateGameObject<GameObject>("Bunny");
		bunny->transform.SetLocalScale(Vector3(0.085f, 0.085f, 0.085f));
		bunny->AddComponent<SkinnedRenderer>()->SetModel(ResourceManager::GetInstance().GetModel("Chibi_Rabbit"));

		auto model = ResourceManager::GetInstance().GetModel("Chibi_Rabbit");
		ResourceManager::GetInstance().LoadAnimation("Chibi_Rabbit_IdleA", "C:\\Project\\BunnyUp\\Engine\\resources\\animations\\Anim_Chibi@IdleA.fbx", model.get(), true);
		ResourceManager::GetInstance().LoadAnimation("Chibi_Rabbit_IdleC", "C:\\Project\\BunnyUp\\Engine\\resources\\animations\\Anim_Chibi@IdleC.fbx", model.get(), true);
		ResourceManager::GetInstance().LoadAnimation("Chibi_Rabbit_Walk", "C:\\Project\\BunnyUp\\Engine\\resources\\animations\\Anim_Chibi@Walk.fbx", model.get(), true);
		ResourceManager::GetInstance().LoadAnimation("Chibi_Rabbit_Run", "C:\\Project\\BunnyUp\\Engine\\resources\\animations\\Anim_Chibi@Run.fbx", model.get(), true);

		auto animator = bunny->AddComponent<Animator>();
		animator->Awake();

		animator->RegistAnimation("IdleA", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleA").get());
		animator->RegistAnimation("IdleC", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleC").get());
		animator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Walk").get());
		animator->RegistAnimation("Run", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Run").get());
		animator->PlayAnimation("IdleA");

		m_activeScene = std::move(scene);
	}

	void SceneManager::SetActiveScene(std::unique_ptr<Scene> scene)
	{
		m_activeScene = std::move(scene);
		if (m_activeScene)
		{
			m_activeScene->SceneAwake();
		}
	}

	void SceneManager::ActiveSceneUpdate(float dt)
	{
		if (m_activeScene)
		{
			m_activeScene->SceneStart();
			m_activeScene->SceneUpdate(dt);
		}
	}

	void SceneManager::ActiveSceneFixedUpdate(float fdt)
	{
		if (m_activeScene)
		{
			m_activeScene->SceneStart();
			m_activeScene->SceneFixedUpdate(fdt);
		}
	}

	void SceneManager::ActiveSceneRender(Renderer& renderer)
	{
		if (m_activeScene)
		{
			m_activeScene->Render(renderer);
		}
	}
}