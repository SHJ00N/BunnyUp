#pragma once

#include "Singleton.h"
#include "Scene.h"

#include <memory>

namespace Engine
{
	class SceneManager : public Singleton<SceneManager>
	{
	public:
		void LoadScene(const std::string& name);
		void CurrentSceneUpdate(float dt);
		void CurrentSceneFixedUpdate(float fdt);
		void CurrentSceneRender(class Renderer& renderer);

		void Initialize();
		void Clear();
		
		Scene* GetCurrentScene() { return m_currentScene.get(); }
		const Scene* GetCurrentScene() const { return m_currentScene.get(); }
	private:
		std::unique_ptr<Scene> m_currentScene;
		std::unique_ptr<Scene> m_nextScene;
		
		void setCurrentScene(std::unique_ptr<Scene> scene);
	};
}