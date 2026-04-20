#include "SceneManager.h"
#include "ModelRenderer.h"
#include "ResourceManager.h"

namespace Engine
{
	void SceneManager::Initialize()
	{
		auto scene = std::make_unique<Scene>();
		auto bunny = scene->CreateGameObject<GameObject>("Bunny");
		bunny->AddComponent<ModelRenderer>()->SetModel(ResourceManager::GetInstance().GetModel("Chibi_Rabbit"));
		bunny->transform.SetLocalScale(Vector3(0.09f, 0.09f, 0.09f));

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