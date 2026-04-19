#include "GameObject.h"

namespace Engine
{
	GameObject::GameObject(const std::string& name) : m_name(name), parent(nullptr), transform()
	{
		transform.owner = this;
	}

	std::unique_ptr<GameObject> GameObject::RemoveChild(GameObject* node)
	{
		for (size_t i = 0; i < children.size(); ++i)
		{
			if (children[i].get() == node)
			{
				node->parent = nullptr;
				node->transform.parentTransform = nullptr;
				node->PropagateTransformDirtyFlag();

				auto removed = std::move(children[i]);

				// Swap with last element
				if (i != children.size() - 1)
				{
					children[i] = std::move(children.back());
				}
				children.pop_back();

				return removed;
			}
		}

		return nullptr;
	}

	void GameObject::Awake()
	{
		// Call Awake on all components
		for (const auto& component : m_components)
		{
			component->Awake();
		}
	}

	void GameObject::Start()
	{
		// Call Start on all components
		for (const auto& component : m_components)
		{
			component->Start();
		}
	}

	void GameObject::Update(float dt)
	{
		// Call Update on all components
		for (const auto& component : m_components)
		{
			component->Update(dt);
		}
	}

	void GameObject::PropagateTransformDirtyFlag()
	{
		transform.SetDirty();
		for (auto& child : children)
		{
			child->PropagateTransformDirtyFlag();
		}
	}
}