#include "SceneManager.h"
#include "SkinnedRenderer.h"
#include "ResourceManager.h"
#include "SceneFactory.h"
#include "Animator.h"
#include "Log.h"

namespace Engine
{
	void SceneManager::Initialize()
	{
		LoadScene("DemoScene1");
	}

	void SceneManager::LoadScene(const std::string& name)
	{
		auto scene = SceneFactory::GetInstance().CreateScene(name);
		if (!scene)
		{
			LOG_ERROR("Failed to load scene : %s", name);
			return;
		}

		scene->SetSceneRootName(name);
		m_nextScene = std::move(scene);
	}

	void SceneManager::setCurrentScene(std::unique_ptr<Scene> scene)
	{
		if (m_currentScene)
		{
			m_currentScene->SceneExit(); // scene clear
		}

		m_currentScene = std::move(scene);

		if (m_currentScene)
		{
			m_currentScene->SceneAwake(); // scene initialize
		}
	}

	void SceneManager::CurrentSceneUpdate(float dt)
	{
		if (m_nextScene)
		{
			setCurrentScene(std::move(m_nextScene));
		}

		if (m_currentScene)
		{
			m_currentScene->SceneStart();
			m_currentScene->SceneUpdate(dt);
		}
	}

	void SceneManager::CurrentSceneFixedUpdate(float fdt)
	{
		if (m_currentScene)
		{
			m_currentScene->SceneStart();
			m_currentScene->SceneFixedUpdate(fdt);
		}
	}

	void SceneManager::CurrentSceneRender(Renderer& renderer)
	{
		if (m_currentScene)
		{
			m_currentScene->Render(renderer);
		}
	}

	void SceneManager::Clear()
	{
		if (m_currentScene)
		{
			m_currentScene->SceneExit();
		}
		m_currentScene.reset();
	}
}