#pragma once

#include "Component.h"
#include "Transform.h"

#include <memory>
#include <vector>
#include <string>

namespace Engine
{
	class GameObject
	{
	public:
		GameObject(const std::string& name = "GameObject");
		~GameObject() = default;

		// Space information
		Transform transform;

		// Propagate dirty flag to children when this node's transform is modified
		void PropagateTransformDirtyFlag();

		// Tree management functions
		void AddChild(std::unique_ptr<GameObject> child)
		{
			// Set parent-child relationship
			child->parent = this;
			child->transform.parentTransform = &this->transform;

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

		const std::string& GetName() const { return m_name; }
		void SetName(const std::string& name) { m_name = name; }

	protected:
		std::string m_name;

		// Scene graph
		GameObject* parent;
		std::vector<std::unique_ptr<GameObject>> children;

		// Components
		std::vector<std::unique_ptr<Component>> m_components;
	};
}