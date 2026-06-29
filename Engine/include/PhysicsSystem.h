// license ---------------------------------------------------------------------------------
// This class is wrapper for ReactPhysics3d
// ReactPhysics3d : https://github.com/DanielChappuis/reactphysics3d
// -------------------------------------------------------------------------------------------

#pragma once

#pragma comment(lib, "reactphysics3d.lib")

#include <reactphysics3d/reactphysics3d.h>
#include <unordered_map>
#include <unordered_set>

#include "EventBus.h"
#include "Rp3dCollisionPair.h"
#include "CollisionEventListener.h"
#include "CollisionMask.h"

namespace Engine
{
	class PhysicsSystem
	{
	public:
		bool DebugRenderEnabled;
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
		const CollisionMatrix& GetCollisionMatrix() const { return m_collisionMatrix; }

		// setter
		void SetDebugDraw(bool value);
		void SetCollisionLayer(CollisionLayer a, CollisionLayer b, bool enable);

		// rigidbody utility
		void AddRigidbody(Rp3dRigidbody* rigidbody);
		void RemoveRigidbody(Rp3dRigidbody* rigidbody);

		void DrawDebug();
	private:
		rp3d::PhysicsCommon m_common;
		rp3d::PhysicsWorld* m_world;
		CollisionEventListener m_collisionEventListener;

		std::unordered_map<uint64_t, class Rp3dRigidbody*> m_bodies;

		std::unordered_set<Rp3dCollisionPair, Rp3dCollisionPairHash> m_currentPairs;
		std::unordered_set<Rp3dCollisionPair, Rp3dCollisionPairHash> m_previousPairs;

		CollisionMatrix m_collisionMatrix;

		ListenerID m_collisionCallbackListenerID;
		ListenerID m_objectDestroyedListenerID;

		void customizeCollisionMatrix();
		void processCollisionEvents();
		void removeCollisionPair(Rp3dCollider* collider);
	};
}