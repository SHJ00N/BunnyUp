#include "pch.h"
#include "GameObject.h"
#include "EventBus.h"
#include "Rp3dRigidbody.h"

namespace Engine
{
	GameObject::GameObject(const std::string& name) : m_name(name), parent(nullptr), transform(), scene(nullptr), m_isDestroyed(false)
	{
		transform.owner = this;
	}

	GameObject::~GameObject()
	{
		Destroy();
	}

	std::unique_ptr<GameObject> GameObject::RemoveChild(GameObject* node)
	{
		for (size_t i = 0; i < children.size(); ++i)
		{
			if (children[i].get() == node)
			{
				assert(node->parent == this);
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

	void GameObject::DestroyObject()
	{
		while (!children.empty())
		{
			children.back()->DestroyObject();
		}

		EventBus::GetInstance().Publish<ObjectDestroyedEvent>({ this });

		if (parent)
		{
			parent->RemoveChild(this);
		}
	}

	void GameObject::SetScene(Scene* scene)
	{
		this->scene = scene;

		for (auto& child : children)
		{
			child->SetScene(scene);
		}
	}

	void GameObject::SetActive(bool value)
	{
		m_isActive = value;
		auto* rigidbody = GetComponent<Rp3dRigidbody>();
		if (rigidbody)
		{
			rigidbody->SetActive(value);
		}
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

	void GameObject::FixedUpdate(float fdt)
	{
		// Call FixedUpdate on all components
		for (const auto& component : m_components)
		{
			component->FixedUpdate(fdt);
		}
	}
	
	void GameObject::Destroy()
	{
		if (!m_isDestroyed)
		{
			m_isDestroyed = true;
			// Call Destroy on all components
			for (const auto& component : m_components)
			{
				component->Destroy();
			}

			for (const auto& child : children)
			{
				child->Destroy();
			}
		}
	}

	void GameObject::OnTriggerEnter(Rp3dCollider* other)
	{
		// Call OnTriggerEnter on all components
		for (const auto& component : m_components)
		{
			component->OnTriggerEnter(other);
		}
	}

	void GameObject::OnTriggerStay(Rp3dCollider* other)
	{
		// Call OnTriggerStay on all components
		for (const auto& component : m_components)
		{
			component->OnTriggerStay(other);
		}
	}

	void GameObject::OnTriggerExit(Rp3dCollider* other)
	{
		// Call OnTriggerExit on all components
		for (const auto& component : m_components)
		{
			component->OnTriggerExit(other);
		}
	}

	void GameObject::OnCollisionEnter(CollisionData data)
	{
		// Call OnCollisionEnter on all components
		for (const auto& component : m_components)
		{
			component->OnCollisionEnter(data);
		}
	}

	void GameObject::OnCollisionStay(CollisionData data)
	{
		// Call OnCollisionStay on all components
		for (const auto& component : m_components)
		{
			component->OnCollisionStay(data);
		}
	}

	void GameObject::OnCollisionExit(CollisionData data)
	{
		// Call OnCollisionExit on all components
		for (const auto& component : m_components)
		{
			component->OnCollisionExit(data);
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

	bool GameObject::IsAncestorOf(const GameObject* node) const
	{
		while (node)
		{
			if (node == this)
				return true;

			node = node->parent;
		}

		return false;
	}
}