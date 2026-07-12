#pragma once

#include "CollisionData.h"

namespace Engine
{
	class GameObject;
	class Rp3dCollider;
	class Collider;

	class Component
	{
	public:
		virtual ~Component() = default;
		
		// Owner GameObject pointer
		GameObject* ownerGameObject = nullptr;

		// Lifecycle methods
		void Awake();
		void Start();
		void Destroy();
		virtual void Update(float dt) { }
		virtual void FixedUpdate(float fdt) { }
		
		bool isEnabled() const { return m_isEnabled; }
		void SetEnabled(bool value) { m_isEnabled = value; }

		// physics methods
		virtual void OnCollisionEnter(CollisionData data) { }
		virtual void OnCollisionStay(CollisionData data) { }
		virtual void OnCollisionExit(CollisionData data) { }

		virtual void OnTriggerEnter(Rp3dCollider* other) { }
		virtual void OnTriggerStay(Rp3dCollider* other) { }
		virtual void OnTriggerExit(Rp3dCollider* other) { }

		// legacy
		virtual void OnCollisionEnter(Collider* other) {}
		virtual void OnCollisionStay(Collider* other) {}
		virtual void OnCollisionExit(Collider* other) {}

		virtual void OnTriggerEnter(Collider* other) {}
		virtual void OnTriggerStay(Collider* other) {}
		virtual void OnTriggerExit(Collider* other) {}

		virtual void OnImGui() { }
	protected:
		// Override these in derived classes for custom behavior
		virtual void OnAwake() { }
		virtual void OnStart() { }
		virtual void OnDestroy() { }

	private:
		bool m_isEnabled = true;
		bool m_isAwake = false;
		bool m_isStarted = false;
		bool m_isDestroyed = false;
	};
}