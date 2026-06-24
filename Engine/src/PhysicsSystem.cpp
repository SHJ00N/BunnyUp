#include "pch.h"
#include "PhysicsSystem.h"
#include "Rp3dRigidbody.h"
#include "Log.h"

namespace Engine
{
	PhysicsSystem::PhysicsSystem() : m_world(nullptr)
	{
	}

	PhysicsSystem::~PhysicsSystem()
	{
		// prevent memory leak
		Shutdown();
	}

	void PhysicsSystem::Initialize()
	{
		m_rigidbodyCreateListenerID = EventBus::GetInstance().Subscribe<RigidbodyCreateEvent>(
			[this](const RigidbodyCreateEvent& e)
			{
				if (e.body)
				{
					m_bodies[e.body->GetId()] = e.body;
				}
			}
		);

		m_rigidbodyDestroyListenerID = EventBus::GetInstance().Subscribe<RigidbodyDestroyEvent>(
			[this](const RigidbodyDestroyEvent& e)
			{
				if (e.body)
				{
					m_bodies.erase(e.body->GetId());
				}
			}
		);

		// already exist
		if (m_world) return;

		// create a physics world
		rp3d::PhysicsWorld::WorldSettings settings;
		settings.defaultVelocitySolverNbIterations = 20;
		settings.isSleepingEnabled = false;
		settings.gravity = rp3d::Vector3(0, -9.81, 0);
		m_world = m_common.createPhysicsWorld(settings);

		// setting world
		// Change the number of iterations of the velocity solver. default value is 10
		m_world->setNbIterationsVelocitySolver(10);
		// Change the number of iterations of the position solver. default value is 5
		m_world->setNbIterationsPositionSolver(5);
		// Set state the sleeping technique. default is true
		m_world->enableSleeping(false);
	}

	void PhysicsSystem::Shutdown()
	{
		EventBus::GetInstance().Unsubscribe<RigidbodyCreateEvent>(m_rigidbodyCreateListenerID);
		EventBus::GetInstance().Unsubscribe<RigidbodyDestroyEvent>(m_rigidbodyDestroyListenerID);

		// world is null
		if (!m_world) return;
		// destory the world
		m_common.destroyPhysicsWorld(m_world);
		m_world = nullptr;
	}

	void PhysicsSystem::Update(float fdt)
	{
		if (!m_world) return;

		// synchronizing game object transform to rp3d rigidBody transform
		for (auto body : m_bodies)
		{
			auto rigidbody = body.second;
			if (rigidbody && rigidbody->GetType() != BodyType::STATIC)
			{
				body.second->SyncTransformToPhysics();
			}
		}
		
		m_world->update(fdt);

		// synchronizing rp3d rigidBody transform to game object transform
		for (auto body : m_bodies)
		{
			auto rigidbody = body.second;
			if (rigidbody && rigidbody->GetType() == BodyType::DYNAMIC)
			{
				body.second->SyncPhysicsToTransform();
			}
		}
	}
}