#pragma once

#include "Component.h"
#include "Transform.h"
#include "ObjectTag.h"

#include <memory>
#include <vector>
#include <string>

namespace Engine
{
	class Scene;
	class Collider;
	class Rp3dCollider;

	class GameObject
	{
	public:
		GameObject(const std::string& name = "GameObject");
		~GameObject();

		// Space information
		Transform transform;

		// owner scene
		Scene* scene = nullptr;

		// Propagate dirty flag to children when this node's transform is modified
		void PropagateTransformDirtyFlag();

		// Add children game object
		template<typename T, typename... Args>
		T* CreateGameObject(Args&&... args)
		{
			// Ensure T is derived from SceneNode
			static_assert(std::is_base_of<GameObject, T>(), "T must be a GameObject");

			// Create a new GameObject and add it to the root
			auto object = std::make_unique<T>(std::forward<Args>(args)...);
			T* ptr = object.get();

			// Set parent-child relationship
			AddChild(std::move(object));

			return ptr;
		}

		// Tree management functions
		void AddChild(std::unique_ptr<GameObject> child)
		{
			// Set parent-child relationship
			child->parent = this;
			child->transform.parentTransform = &this->transform;

			if (this->scene)
			{
				child->SetScene(this->scene);
			}

			children.emplace_back(std::move(child));
		}

		std::unique_ptr<GameObject> RemoveChild(GameObject* node);

		// Accessors
		std::vector<std::unique_ptr<GameObject>>& GetChildren() { return children; }
		// Read-only version for external access
		const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return children; }

		// Component management
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>(), "T must be a Component");

			// Create new component
			auto component = std::make_unique<T>(std::forward<Args>(args)...);
			component->ownerGameObject = this;

			// Store component and return raw pointer
			T* ptr = component.get();
			m_components.emplace_back(std::move(component));
			return ptr;
		}

		template<typename T>
		T* GetComponent()
		{
			static_assert(std::is_base_of<Component, T>(), "T must be a Component");

			// Search for the first component of type T
			for (const auto& component : m_components)
			{
				auto casted = dynamic_cast<T*>(component.get());
				if (casted)
				{
					return casted;
				}
			}
			return nullptr; // Not found
		}

		std::vector<std::unique_ptr<Component>>& GetComponents() { return m_components; }

		void Awake();
		void Start();
		void Update(float dt);
		void FixedUpdate(float fdt);
		void Destroy();

		void SetTag(ObjectTag tag) { m_tag = tag; }

		void SetScene(Scene* scene);
		
		const std::string& GetName() const { return m_name; }
		void SetName(const std::string& name) { m_name = name; }
		bool IsDestroyed() const { return m_isDestroyed; }
		bool IsAncestorOf(const GameObject* node) const;
		GameObject* GetParent() const { return parent; }
		ObjectTag GetTag() const { return m_tag; }

		// physics event handlers
		void OnTriggerEnter(Rp3dCollider* other);
		void OnTriggerStay(Rp3dCollider* other);
		void OnTriggerExit(Rp3dCollider* other);
		void OnCollisionEnter(Rp3dCollider* other);
		void OnCollisionStay(Rp3dCollider* other);
		void OnCollisionExit(Rp3dCollider* other);

		// legacy
		void OnTriggerEnter(Collider* other);
		void OnTriggerStay(Collider* other);
		void OnTriggerExit(Collider* other);
		void OnCollisionEnter(Collider* other);
		void OnCollisionStay(Collider* other);
		void OnCollisionExit(Collider* other);

	protected:
		std::string m_name;
		bool m_isDestroyed = false;

		ObjectTag m_tag = ObjectTag::None;

		// Scene graph
		GameObject* parent;
		std::vector<std::unique_ptr<GameObject>> children;

		// Components
		std::vector<std::unique_ptr<Component>> m_components;
	};
}