// license ---------------------------------------------------------------------------------
// This class is wrapper for ReactPhysics3d
// ReactPhysics3d : https://github.com/DanielChappuis/reactphysics3d
// -------------------------------------------------------------------------------------------

#pragma once

#pragma comment(lib, "reactphysics3d.lib")

#include <reactphysics3d/reactphysics3d.h>
#include <unordered_map>

#include "EventBus.h"

namespace Engine
{
	class PhysicsSystem
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

		// runtime loop
		void Update(float fdt);
		
		// lifecycle
		void Initialize();
		void Shutdown();

		// getter
		rp3d::PhysicsCommon& GetPhysicsCommon() { return m_common; }
		rp3d::PhysicsWorld* GetPhysicsWorld() const { return m_world; }
	private:
		rp3d::PhysicsCommon m_common;
		rp3d::PhysicsWorld* m_world;

		std::unordered_map<uint64_t, class Rp3dRigidbody*> m_bodies;

		ListenerID m_rigidbodyCreateListenerID;
		ListenerID m_rigidbodyDestroyListenerID;
	};
}