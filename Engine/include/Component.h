#pragma once

namespace Engine
{
	class GameObject;

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

		virtual void OnImGui() { }
	protected:
		// Override these in derived classes for custom behavior
		virtual void OnAwake() { }
		virtual void OnStart() { }
		virtual void OnDestroy() { }

	private:
		bool m_isAwake = false;
		bool m_isStarted = false;
		bool m_isDestroyed = false;
	};
}