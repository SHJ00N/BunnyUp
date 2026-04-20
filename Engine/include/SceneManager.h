#pragma once

#include "Singleton.h"
#include "Scene.h"

#include <memory>

namespace Engine
{
	class SceneManager : public Singleton<SceneManager>
	{
	public:
		void SetActiveScene(std::unique_ptr<Scene> scene);
		void ActiveSceneUpdate(float dt);
		void ActiveSceneFixedUpdate(float fdt);
		void ActiveSceneRender(class Renderer& renderer);

		void Initialize();
		void Clear() { m_activeScene.reset(); }
		
		Scene* GetActiveScene() { return m_activeScene.get(); }
		const Scene* GetActiveScene() const { return m_activeScene.get(); }
	private:
		std::unique_ptr<Scene> m_activeScene;
	};
}