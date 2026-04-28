#pragma once

#include "GameObject.h"

#include <memory>

namespace Engine
{
	class Camera;
	class Scene
	{
	public:
		Scene();
		~Scene();

		void SceneAwake();
		void SceneStart();
		void SceneUpdate(float dt);
		void SceneFixedUpdate(float fdt);
		virtual void SceneExit() { }

		void Render(class Renderer& renderer);

		GameObject* GetRoot() { return m_root.get(); }
		// Read-only version for external access
		const GameObject* GetRoot() const { return m_root.get(); }
		void SetSceneRootName(const std::string& name) { m_root->SetName(name); }

		// Add GameObject to the root of the scene
		template<typename T, typename... Args>
		T* CreateGameObject(Args&&... args)
		{
			// Ensure T is derived from SceneNode
			static_assert(std::is_base_of<GameObject, T>(), "T must be a GameObject");

			// Create a new GameObject and add it to the root
			auto object = std::make_unique<T>(std::forward<Args>(args)...); 
			T* ptr = object.get(); 
			
			// Set parent-child relationship
			m_root->AddChild(std::move(object));
			return ptr;
		}

		// camera management
		void RegisterCamera(Camera* camera);
		void UnregisterCamera(Camera* camera);
		const std::vector<Camera*>& GetCameras() const { return m_cameras; }
		Camera* GetMainCamera() const { return m_mainCamera; }
		void SetMainCamera(Camera* camera) { m_mainCamera = camera; }

	protected:
		virtual void SceneEnter() { }

	private:
		std::unique_ptr<GameObject> m_root;

		std::vector<Camera*> m_cameras;
		Camera* m_mainCamera = nullptr;

		// Helper functions for traversing the scene graph
		void traverseAwake(GameObject* node);
		void traverseStart(GameObject* node);
		void traverseUpdate(GameObject* node, float dt);
		void traverseFixedUpdate(GameObject* node, float fdt);
		void traverseRender(GameObject* node, Renderer& renderer);
	};
}