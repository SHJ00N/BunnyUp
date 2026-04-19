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
		virtual void Update(float dt) { }

		virtual void OnImGui() { }
	protected:
		// Override these in derived classes for custom behavior
		virtual void OnAwake() { }
		virtual void OnStart() { }

	private:
		bool m_isAwake = false;
		bool m_isStarted = false;
	};
}