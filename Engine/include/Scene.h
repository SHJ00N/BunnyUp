#pragma once

#include "GameObject.h"

#include <memory>

namespace Engine
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void SceneAwake();
		void SceneStart();
		void SceneUpdate(float dt);

		void Render(class Renderer& renderer);

		GameObject* GetRoot() { return m_root.get(); }
		// Read-only version for external access
		const GameObject* GetRoot() const { return m_root.get(); }

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

	private:
		std::unique_ptr<GameObject> m_root;

		// Helper functions for traversing the scene graph
		void traverseAwake(GameObject* node);
		void traverseStart(GameObject* node);
		void traverseUpdate(GameObject* node, float dt);
		void traverseRender(GameObject* node, Renderer& renderer);
	};
}