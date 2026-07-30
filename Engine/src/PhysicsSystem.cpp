#include "pch.h"
#include "PhysicsSystem.h"
#include "Rp3dRigidbody.h"
#include "Log.h"
#include "MathHelper.h"
#include "DebugRenderer.h"

namespace Engine
{
	PhysicsSystem::PhysicsSystem() : m_world(nullptr), DebugRenderEnabled(false)
	{
	}

	PhysicsSystem::~PhysicsSystem()
	{
		// prevent memory leak
		Shutdown();
	}

	void PhysicsSystem::Initialize()
	{
		m_collisionCallbackListenerID = EventBus::GetInstance().Subscribe<CollisionCallbackEvent>(
			[this](const CollisionCallbackEvent& e)
			{
				if (e.pair.a == nullptr || e.pair.b == nullptr)
				{
					return;
				}

				m_currentPairs.insert(e.pair);
			}
		);

		m_objectDestroyedListenerID = EventBus::GetInstance().Subscribe<ObjectDestroyedEvent>(
			[this](const ObjectDestroyedEvent& e)
			{
				auto collider = e.object->GetComponent<Rp3dCollider>();
				if (collider)
				{
					removeCollisionPair(collider);
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
		m_world->setNbIterationsVelocitySolver(40);
		// Change the number of iterations of the position solver. default value is 5
		m_world->setNbIterationsPositionSolver(20);
		// Set state the sleeping technique. default is true
		m_world->enableSleeping(true);

		m_world->setEventListener(&m_physicsEventListener);

		// set collision layer
		customizeCollisionMatrix();
	}

	void PhysicsSystem::Shutdown()
	{
		m_previousPairs.clear();
		m_currentPairs.clear();
		m_bodies.clear();

		EventBus::GetInstance().Unsubscribe<CollisionCallbackEvent>(m_collisionCallbackListenerID);
		EventBus::GetInstance().Unsubscribe<ObjectDestroyedEvent>(m_objectDestroyedListenerID);

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

		// swap collision pair list
		m_previousPairs.swap(m_currentPairs);
		m_currentPairs.clear();
		
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

		// dispatch collision events based on current and previous collision pairs
		processCollisionEvents();
	}

	void PhysicsSystem::AddRigidbody(Rp3dRigidbody* rigidbody)
	{
		m_bodies[rigidbody->GetId()] = rigidbody;
	}

	void PhysicsSystem::RemoveRigidbody(Rp3dRigidbody* rigidbody)
	{
		m_bodies.erase(rigidbody->GetId());
	}

	void PhysicsSystem::SetDebugDraw(bool value)
	{
		if (!m_world) return;

		DebugRenderEnabled = value;
		m_world->setIsDebugRenderingEnabled(value);
		rp3d::DebugRenderer& debugRenderer = m_world->getDebugRenderer();
		debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, value);
		debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, value);
	}

	void PhysicsSystem::SetCollisionLayer(CollisionLayer a, CollisionLayer b, bool enable)
	{
		m_collisionMatrix.SetCollision(a, b, enable);
	}

	void PhysicsSystem::DrawDebug()
	{
		if (!m_world || !DebugRenderEnabled) return;

		const auto& debugRenderer = m_world->getDebugRenderer();

		const auto& lines = debugRenderer.getLines();
		for (const auto& line : lines)
		{
			uint32_t color1 = line.color1;
			float r1 = ((color1 >> 24) & 0xFF) / 255.0f;
			float g1 = ((color1 >> 16) & 0xFF) / 255.0f;
			float b1 = ((color1 >> 8) & 0xFF) / 255.0f;
			float a1 = ((color1 >> 0) & 0xFF) / 255.0f;

			uint32_t color2 = line.color2;
			float r2 = ((color2 >> 24) & 0xFF) / 255.0f;
			float g2 = ((color2 >> 16) & 0xFF) / 255.0f;
			float b2 = ((color2 >> 8) & 0xFF) / 255.0f;
			float a2 = ((color2 >> 0) & 0xFF) / 255.0f;


			DebugRenderer::GetInstance().AddLine(
				Vector3(line.point1.x, line.point1.y, line.point1.z),
				Vector3(line.point2.x, line.point2.y, line.point2.z),
				Vector4(r1, g1, b1, a1),
				Vector4(r2, g2, b2, a2));
		}

		const auto& triangles = debugRenderer.getTriangles();
		for (const auto& triangle : triangles)
		{
			uint32_t color1 = triangle.color1;
			float r1 = ((color1 >> 24) & 0xFF) / 255.0f;
			float g1 = ((color1 >> 16) & 0xFF) / 255.0f;
			float b1 = ((color1 >> 8) & 0xFF) / 255.0f;
			float a1 = ((color1 >> 0) & 0xFF) / 255.0f;

			uint32_t color2 = triangle.color2;
			float r2 = ((color2 >> 24) & 0xFF) / 255.0f;
			float g2 = ((color2 >> 16) & 0xFF) / 255.0f;
			float b2 = ((color2 >> 8) & 0xFF) / 255.0f;
			float a2 = ((color2 >> 0) & 0xFF) / 255.0f;

			uint32_t color3 = triangle.color3;
			float r3 = ((color3 >> 24) & 0xFF) / 255.0f;
			float g3 = ((color3 >> 16) & 0xFF) / 255.0f;
			float b3 = ((color3 >> 8) & 0xFF) / 255.0f;
			float a3 = ((color3 >> 0) & 0xFF) / 255.0f;

			DebugRenderer::GetInstance().AddTriangle(
				Vector3(triangle.point1.x, triangle.point1.y, triangle.point1.z),
				Vector3(triangle.point2.x, triangle.point2.y, triangle.point2.z),
				Vector3(triangle.point3.x, triangle.point3.y, triangle.point3.z),
				Vector4(r1, g1, b1, 0.2f),
				Vector4(r2, g2, b2, 0.2f),
				Vector4(r3, g3, b3, 0.2f));
		}

		// draw ray
		for (const auto& ray : m_rayList)
		{
			DebugRenderer::GetInstance().AddLine(ray.start, ray.end, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		}
		// clear ray
		m_rayList.clear();
	}

	void PhysicsSystem::removeCollisionPair(Rp3dCollider* collider)
	{
		std::erase_if(
			m_previousPairs,
			[collider](const Rp3dCollisionPair& pair)
			{
				return pair.a == collider || pair.b == collider;
			});

		std::erase_if(
			m_currentPairs,
			[collider](const Rp3dCollisionPair& pair)
			{
				return pair.a == collider || pair.b == collider;
			});
	}

	void PhysicsSystem::processCollisionEvents()
	{
		// Enter / Stay
		for (const Rp3dCollisionPair& pair : m_currentPairs)
		{
			// check if either collider is a trigger
			const bool isTrigger = pair.a->IsTrigger() || pair.b->IsTrigger();

			CollisionData dataA(pair.b, pair.contact.normal * -1.0f, pair.contact.penetrationDepth);
			CollisionData dataB(pair.a, pair.contact.normal, pair.contact.penetrationDepth);

			const bool existedLastFrame = m_previousPairs.contains(pair);
			if (isTrigger)
			{
				if (existedLastFrame)
				{
					pair.a->ownerGameObject->OnTriggerStay(pair.b);
					pair.b->ownerGameObject->OnTriggerStay(pair.a);
				}
				else
				{
					pair.a->ownerGameObject->OnTriggerEnter(pair.b);
					pair.b->ownerGameObject->OnTriggerEnter(pair.a);
				}
			}
			else
			{
				if (existedLastFrame)
				{
					pair.a->ownerGameObject->OnCollisionStay(dataA);
					pair.b->ownerGameObject->OnCollisionStay(dataB);
				}
				else
				{
					pair.a->ownerGameObject->OnCollisionEnter(dataA);
					pair.b->ownerGameObject->OnCollisionEnter(dataB);
				}
			}
		}

		// Exit
		for (const Rp3dCollisionPair& pair : m_previousPairs)
		{
			if (m_currentPairs.contains(pair))
			{
				continue;
			}

			CollisionData dataA(pair.b, pair.contact.normal * -1.0f, pair.contact.penetrationDepth);
			CollisionData dataB(pair.a, pair.contact.normal, pair.contact.penetrationDepth);

			const bool isTrigger = pair.a->IsTrigger() || pair.b->IsTrigger();
			if (isTrigger)
			{
				pair.a->ownerGameObject->OnTriggerExit(pair.b);
				pair.b->ownerGameObject->OnTriggerExit(pair.a);
			}
			else
			{
				pair.a->ownerGameObject->OnCollisionExit(dataA);
				pair.b->ownerGameObject->OnCollisionExit(dataB);
			}
		}
	}

	void PhysicsSystem::customizeCollisionMatrix()
	{
		SetCollisionLayer(CollisionLayer::Player, CollisionLayer::Player, false);
		SetCollisionLayer(CollisionLayer::Player, CollisionLayer::Enemy, false);
		SetCollisionLayer(CollisionLayer::Player, CollisionLayer::PlayerTrigger, false);
		SetCollisionLayer(CollisionLayer::Enemy, CollisionLayer::Enemy, false);
		SetCollisionLayer(CollisionLayer::Enemy, CollisionLayer::EnemyTrigger, false);
		SetCollisionLayer(CollisionLayer::Ground, CollisionLayer::Ground, false);
		SetCollisionLayer(CollisionLayer::Wall, CollisionLayer::Wall, false);
		SetCollisionLayer(CollisionLayer::Ground, CollisionLayer::Wall, false);
		
		SetCollisionLayer(CollisionLayer::Trigger, CollisionLayer::PlayerTrigger, false);
		SetCollisionLayer(CollisionLayer::Trigger, CollisionLayer::EnemyTrigger, false);
	}

	bool PhysicsSystem::Raycast(const Vector3& origin, const Vector3& direction, float maxDistance, RaycastHit& outHitInfo)
	{
		outHitInfo = {};

		Vector3 dir = Normalize(direction);
		Normalize(dir);
		const Vector3 end = origin + dir * maxDistance;

		// add ray for debug
		m_rayList.push_back({ origin, end, maxDistance });

		rp3d::Ray ray(rp3d::Vector3(origin.x, origin.y, origin.z), rp3d::Vector3(end.x, end.y, end.z));
		RaycastCallback callback(outHitInfo, maxDistance);

		m_world->raycast(ray, &callback);

		return outHitInfo.collider != nullptr;
	}
}